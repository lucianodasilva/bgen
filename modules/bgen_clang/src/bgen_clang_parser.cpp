#include "bgen_clang_parser.h"
#include "bgen_clang_tools.h"

#include <list>
#include <vector>

using namespace std;
using namespace bgen::source;

namespace bgen {
    namespace clang {

        const string pre_compiled_headers = "bgen.pch";

        namespace internal {

            inline bool is_valid_kind(type_kind kind) {
                return kind != type_kind::type_kind_invalid && kind != type_kind::type_kind_unhandled;
            }

            struct visitor_context {
                context & cxt;
                parser_result & result;
            };

            size_t make_type (visitor_context & visitor_cxt, const CXType & native_type);

            inline void handle_inplace_struct (visitor_context & visitor_cxt, const CXType & src_type, bgen::source::type & type) {
                auto decl_cursor = clang_getTypeDeclaration (src_type);
                auto decl_type = clang_getCursorType(decl_cursor);

                string native_name = tools::get_spelling(decl_type);
                string stct_name = tools::get_spelling (decl_cursor);

                bool is_struct = decl_cursor.kind == CXCursor_StructDecl || decl_cursor.kind == CXCursor_ClassDecl;

                if (!is_struct)
                    return;

                type.kind = type_kind::type_kind_struct;
                type.base_struct_id = visitor_cxt.result.structs.find_or_emplace (native_name).first;
                
                visitor_cxt.result.structs [type.base_struct_id].name = native_name;

                // extract namespace
                list < string > nspace_info;
                CXCursor parent = clang_getCursorSemanticParent(decl_cursor);

                while (parent.kind == CXCursorKind::CXCursor_Namespace) {
                    nspace_info.push_front (tools::get_spelling(parent));
                    parent = clang_getCursorSemanticParent(parent);
                }

                visitor_cxt.result.structs [type.base_struct_id].namespace_name = {nspace_info.begin (), nspace_info.end ()};

                int t_num = clang_Type_getNumTemplateArguments(src_type);
                if (t_num != 0) {
                    for (int ti = 0; ti < t_num; ++ti) {
                        auto ttype = clang_Type_getTemplateArgumentAsType(src_type, ti);
                        if (ttype.kind == CXType_Invalid) {
                            visitor_cxt.cxt.status.warn (tools::get_location (decl_cursor)) << "unsupported template argument kind";
                            continue;
                        }

                        type.template_params.push_back (template_param_info { make_type(visitor_cxt, ttype)});
                    }
                }
            }

            size_t make_type (visitor_context & visitor_cxt, const CXType & native_type) {
                CXType src_type = native_type;

                // unfold canonical type
                while (src_type.kind == CXType_Typedef)
                    src_type = clang_getCanonicalType(src_type);

                string native_name = tools::get_spelling(src_type);

                auto type_id = visitor_cxt.result.types.find_index (native_name);

                if (type_id == type_vector::nindex) {

                    auto & type = visitor_cxt.result.types [type_id];

                    type.dimention = 0;
                    type.is_const = clang_isConstQualifiedType(src_type) != 0;
                    type.kind = tools::convert_type_kind(src_type.kind);
                    type.name = native_name;

                    // handle specificities
                    switch (type.kind) {
                        case (type_kind::type_kind_pointer) :
                        case (type_kind::type_kind_lvalue_ref) :
                        case (type_kind::type_kind_rvalue_ref) :
                            type.base_type_id = make_type (visitor_cxt, clang_getPointeeType(src_type));
                            break;
                        case (type_kind::type_kind_struct) :
                            handle_inplace_struct(visitor_cxt, src_type, type);
                            break;
                        case (type_kind::type_kind_unhandled) :
                            {
                                int t_num = clang_Type_getNumTemplateArguments(src_type);
                                if (t_num != 0)
                                    handle_inplace_struct(visitor_cxt, src_type, type);
                            }
                            break;
                        case (type_kind::type_kind_enum) :
                            // still not supported
                            type.kind = type_kind::type_kind_unhandled;
                            break;
                        case (type_kind::type_kind_constant_array) :
                            type.dimention = static_cast <uint32_t> (clang_getNumElements(src_type));
                            type.base_type_id = make_type (visitor_cxt, clang_getArrayElementType(src_type));
                            break;
                        case (type_kind::type_kind_incomplete_array) :
                            type.base_type_id = make_type (visitor_cxt, clang_getArrayElementType(src_type));
                            break;
                        default:
                            break;
                    }
                }

                return type_id;
            }

            static inline bool check_for_errors(const CXTranslationUnit &tu) {
                auto diag_count = clang_getNumDiagnostics(tu);

                for (uint32_t i = 0; i < diag_count; ++i) {
                    auto diag = clang_getDiagnostic(tu, i);

                    auto severity = clang_getDiagnosticSeverity(diag);

                    if (severity == CXDiagnostic_Error || severity == CXDiagnostic_Fatal)
                        return true;
                }

                return false;
            }

            /* tools */
            static inline void report_diagnostics(context &cxt, const CXTranslationUnit &tu) {
                auto diag_count = clang_getNumDiagnostics(tu);

                for (uint32_t i = 0; i < diag_count; ++i) {
                    auto diag = clang_getDiagnostic(tu, i);

                    location loc = tools::get_location(diag);
                    string message = tools::get_spelling(diag);

                    cxt.status.warn (loc) << message;
                }
            }

            inline vector<const char *>
            make_clang_arguments(const vector<string> &definitions, const vector<string> &includes) {
                vector<const char *> arguments = {
                        "-fsyntax-only", "-x", "c++", "-std=c++14" // defaults ( set clang to not compile only build ast )
                };

                for (auto &d : definitions) {
                    arguments.push_back("-D");
                    arguments.push_back(d.c_str());
                }

                for (auto &i : includes) {
                    arguments.push_back("-I");
                    arguments.push_back(i.c_str());
                }

                return arguments;
            }

            inline CXTranslationUnit create_translation_unit(
                    const CXIndex &index,
                    const string &source,
                    vector<const char *> arguments,
                    bool is_first
            ) {
                if (is_first) {
                    return clang_parseTranslationUnit(
                            index,
                            source.c_str(),
                            arguments.data(),
                            static_cast < int > (arguments.size()),
                            nullptr,
                            0,
                            CXTranslationUnit_ForSerialization |
                            CXTranslationUnit_SkipFunctionBodies |
                            CXTranslationUnit_DetailedPreprocessingRecord
                    );
                } else {
                    return clang_createTranslationUnitFromSourceFile(
                            index,
                            source.c_str(),
                            static_cast < int > (arguments.size()),
                            arguments.data(),
                            0,
                            nullptr
                    );
                }
            }

            CXChildVisitResult visit_child (
                CXCursor cursor,
                CXCursor parent,
                CXClientData client_data
            ) {
                if (clang_Location_isFromMainFile(clang_getCursorLocation(cursor)) == 0)
                    return CXChildVisit_Continue;

                CXCursorKind native_cursor_kind = clang_getCursorKind(cursor);
                element_type kind = tools::convert_kind(native_cursor_kind);

                if (kind == element_type::undefined) {
                    // LOG
                    return CXChildVisitResult::CXChildVisit_Continue; 
                }

                auto & visitor_cxt = *(visitor_context *)client_data;
                auto & result = visitor_cxt.result;
                // create element
                result.elements.emplace_back ();
                auto & element = result.elements.back();

                element.location = tools::get_location (cursor);
                element.name = tools::get_spelling(cursor);
                element.element_type = kind;

                // parse flags
                element.flags.is_virtual = clang_CXXMethod_isVirtual(cursor) != 0;
                element.flags.is_pure = clang_CXXMethod_isPureVirtual(cursor) != 0;
                element.flags.is_static = clang_CXXMethod_isStatic(cursor) != 0;
                element.flags.is_const = clang_CXXMethod_isConst(cursor) != 0;
                element.flags.is_ctor = (native_cursor_kind == CXCursor_Constructor);

                // handle type
                auto native_type = clang_getCursorType(cursor);
                element.type_id = make_type(visitor_cxt, native_type);

                // visit further children
                clang_visitChildren(cursor, &visit_child, client_data);

                return CXChildVisitResult::CXChildVisit_Continue;
            }
        }


        parser_result parser::parse ( bgen::context & cxt, const bgen::parameters & params) {
            parser_result result {};

            auto pch_file = bgen::system::merge_path (params.client_dest, pre_compiled_headers);
            auto include_search_paths = params.include_paths;

            // check for default include search paths
            if (!params.no_default_includes) {
                auto default_includes = bgen::system::get_include_paths ();

                include_search_paths.reserve (
                    include_search_paths.size () + default_includes.size ()
                );

                include_search_paths.insert (
                    include_search_paths.end (),
                    default_includes.begin (),
                    default_includes.end ()
                );
            }

            // build clang argument list
            auto args = internal::make_clang_arguments(params.include_files, include_search_paths);

            auto index = clang_createIndex(0, 0);
            auto_guard([&index]() {
                clang_disposeIndex(index);
            });

            bool is_first_file = true;
            // for each source file
            for (auto & source : params.source_files) {

                CXTranslationUnit tu = internal::create_translation_unit (
                    index,
                    source,
                    args,
                    is_first_file
                );

                if (is_first_file) {
                    args.push_back("-include-pch");
                    args.push_back(pch_file.c_str ());
                    is_first_file = false;
                }

                auto_guard([&tu]() {
                    clang_disposeTranslationUnit(tu);
                });

                auto diag_count = clang_getNumDiagnostics(tu);

                // check for diagnostics
                if (diag_count) {
                    //TODO: diagnostic messages outside the set
                    //		of source files should not be considered
                    internal::report_diagnostics(cxt, tu);

                    if (internal::check_for_errors (tu)) {
                        internal::report_diagnostics (cxt, tu);
                        return {};
                    }
                }

                // list types in source file
                auto cursor = clang_getTranslationUnitCursor(tu);

                internal::visitor_context visitor_cxt = { cxt, result };

                clang_visitChildren(cursor, internal::visit_child, &visitor_cxt);

                // clean up
                clang_saveTranslationUnit(tu, pch_file.c_str(), clang_defaultSaveOptions(tu));
            }

            return result;
        }
    }
}