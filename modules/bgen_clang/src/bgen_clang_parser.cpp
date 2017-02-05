#include "bgen_clang_parser.h"
#include "bgen_clang_tools.h"

#include <vector>

using namespace std;
using namespace bgen::source;

namespace bgen {
    namespace clang {
        namespace internal {

            type_id_t shallow_type (parser_result & result, const CXType & native_type) {
                CXType src_type = native_type;

                while (src_type.kind == CXType_Typedef)
                    src_type = clang_getCanonicalType(src_type);

                string native_name = clang::get_spelling(src_type);

                bool is_new_type = cxt.symbols.types.find (native_name) == cxt.symbols.types.end ();
                auto type = get_or_make_type (cxt.symbols, native_name);

                if (is_new_type) {

                    type->dimention = 0;
                    type->is_const = clang_isConstQualifiedType(src_type) != 0;
                    type->kind = handle_type_kind(cxt, src_type);
                    type->native_name = native_name;

                    switch (type->kind) {
                        case (type_kind::type_kind_pointer) :
                        case (type_kind::type_kind_lvalue_ref) :
                        case (type_kind::type_kind_rvalue_ref) :
                            type->base_type = make_type (cxt, clang_getPointeeType(src_type));
                            break;
                        case (type_kind::type_kind_struct) :
                            handle_inplace_struct(cxt, src_type, type);
                            break;
                        case (type_kind::type_kind_unhandled) : {
                            int t_num = clang_Type_getNumTemplateArguments(src_type);
                            if (t_num != 0)
                                handle_inplace_struct(cxt, src_type, type);
                        }
                            break;
                        case (type_kind::type_kind_enum) :
                            // still not supported
                            type->kind = type_kind::type_kind_unhandled;
                            break;
                        case (type_kind::type_kind_constant_array) :
                            type->dimention = static_cast <uint32_t> (clang_getNumElements(src_type));
                            type->base_type = make_type (cxt, clang_getArrayElementType(src_type));
                            break;
                        case (type_kind::type_kind_incomplete_array) :
                            type->base_type = make_type (cxt, clang_getArrayElementType(src_type));
                            break;
                        default:
                            break;
                    }
                }

                return type;
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

            CXChildVisitResult visit_child (
                CXCursor cursor,
                CXCursor parent,
                CXClientData client_data
            ) {
                if (clang_Location_isFromMainFile(clang_getCursorLocation(cursor)) == 0)
                    return CXChildVisit_Continue;

                auto & result = *(parser_result *)client_data;
                auto kind = clang_getCursorKind(cursor);

                if (kind == element_type::undefined) {
                    // TODO: should log?
                    return CXChildVisitResult::CXChildVisit_Continue;
                }

                // create element
                result.elements.emplace_back ();
                auto & element = result.elements.back();

                element.location = bgen::clang::get_location (cursor);
                element.name = bgen::clang::get_spelling(cursor);
                element.element_type = convert_kind(kind);

                // parse flags
                element.flags.is_virtual = clang_CXXMethod_isVirtual(cursor) != 0;
                element.flags.is_pure = clang_CXXMethod_isPureVirtual(cursor) != 0;
                element.flags.is_static = clang_CXXMethod_isStatic(cursor) != 0;
                element.flags.is_const = clang_CXXMethod_isConst(cursor) != 0;
                element.flags.is_ctor = (kind == CXCursor_Constructor);

                // handle type

                // visit further children
                clang_visitChildren(cursor, &visit_child, client_data);

                return CXChildVisitResult::CXChildVisit_Continue;
            }
        }

        parser_result parser::parse ( const bgen::parameters & params) {
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

                cxt.active_source_file = source;

                CXTranslationUnit tu = internal::create_translation_unit(
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
                    error_status::warn ();

                    if (internal::check_for_errors (tu)) {
                        error_status::fail ();
                        return {};
                    }
                }

                // list types in source file
                auto cursor = clang_getTranslationUnitCursor(tu);
                clang_visitChildren(cursor, internal::visit_child, &result);

                // clean up
                clang_saveTranslationUnit(tu, pch_file.c_str(), clang_defaultSaveOptions(tu));
            }

            return result;
        }
    }
}