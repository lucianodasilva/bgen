//
// Created by Luciano Silva on 15/12/16.
//
#include "bgen_clang_handlers.h"
#include "bgen_clang.h"
#include "bgen_clang_type_maker.h"

#include <memory>
#include <map>

using namespace std;

namespace bgen {
    namespace clang {

        inline bool is_valid_kind(const type_info *info) {
            return info->kind != type_kind::type_kind_invalid && info->kind != type_kind::type_kind_unhandled;
        }

        namespace handlers {

            void child_handler::visit_start(visitor_context &cxt, const CXCursor &cursor) {}

            void child_handler::visit_end(visitor_context &cxt, const CXCursor &cursor) {}

            // namespace_handler
            void namespace_handler::visit_start(visitor_context &cxt, const CXCursor &cursor) {
                cxt.active_namespace.push_back(clang::get_spelling(cursor));
            }

            void namespace_handler::visit_end(visitor_context &cxt, const CXCursor &cursor) {
                cxt.active_namespace.pop_back();
            }

            // fields definition handler
            void field_handler::visit_start(visitor_context &cxt, const CXCursor &cursor) {
                CX_CXXAccessSpecifier access = clang_getCXXAccessSpecifier(cursor);

                visibility_type vis = visibility_type::visibility_private;

                switch (access) {
                    case CX_CXXProtected:
                        vis = visibility_type::visibility_protected;
                        break;
                    case CX_CXXPublic:
                        vis = visibility_type::visibility_public;
                        break;
                    default:
                        break;
                }

                auto name = clang::get_spelling(cursor);
                auto native_type = clang_getCursorType(cursor);

                auto type = make_type(cxt, native_type);

                if (!is_valid_kind(type)) {
                    logger::write(clang::get_location(cursor)) << "invalid or unexpected type for field";
                    return;
                }

                cxt.active_struct->fields.emplace_back();

                auto &f = cxt.active_struct->fields.back();

                f.namespace_name = cxt.active_namespace;
                f.name = name;
                f.type = type;
                f.location = clang::get_location(cursor);
                f.visibility = vis;

            }

            // param definition handler
            void param_handler::visit_start(visitor_context &cxt, const CXCursor &cursor) {
                auto native_type = clang_getCursorType(cursor);
                auto type = make_type(cxt, native_type);

                cxt.active_method.params.emplace_back();
                auto &p = cxt.active_method.params.back();

                p.name = clang::get_spelling(cursor);
                p.type = type;
            }

            // method definition handler
            void method_handler_base::method_visit_start(visitor_context &cxt, const CXCursor &cursor,
                                                         bool is_constructor) {

                if (!cxt.active_struct)
                    return;

                bool is_global = cxt.active_struct == cxt.symbols.global.get();

                visibility_type vis = visibility_type::visibility_private;

                if (is_global) {
                    vis = visibility_type::visibility_public;
                } else {
                    CX_CXXAccessSpecifier access = clang_getCXXAccessSpecifier(cursor);

                    if (access == CX_CXXProtected)
                        vis = visibility_type::visibility_protected;

                    if (access == CX_CXXPublic)
                        vis = visibility_type::visibility_public;
                }

                cxt.active_method = {};

                cxt.active_method.name = clang::get_spelling(cursor);
                cxt.active_method.namespace_name = cxt.active_namespace;
                cxt.active_method.location = clang::get_location(cursor);
                cxt.active_method.visibility = vis;
                cxt.active_method.is_virtual = clang_CXXMethod_isVirtual(cursor) != 0;
                cxt.active_method.is_pure = clang_CXXMethod_isPureVirtual(cursor) != 0;
                cxt.active_method.is_static = clang_CXXMethod_isStatic(cursor) != 0 || is_global;
                cxt.active_method.is_const = clang_CXXMethod_isConst(cursor) != 0;
                cxt.active_method.is_ctor = is_constructor;

                if (!is_constructor) {
                    // unfold return type
                    auto ftype = clang_getCursorType(cursor);
                    auto rtype = clang_getResultType(ftype);

                    cxt.active_method.return_type = make_type(cxt, rtype);
                }
            }

            void method_handler_base::method_visit_end(visitor_context &cxt, const CXCursor &cursor,
                                                       bool is_constructor) {
                if (!cxt.active_struct)
                    return;

                if (!is_constructor) {
                    if (!is_valid_kind(cxt.active_method.return_type)) {
                        logger::write(clang::get_location(cursor)) << "invalid or unexpected type for method";
                        return;
                    }
                }

                // add namespace for use with globals
                cxt.active_method.namespace_name = cxt.active_namespace;

                // TODO: check for validity of parameters
                cxt.active_struct->methods.push_back(cxt.active_method);
            }

            void ctor_method_handler::visit_start(visitor_context &cxt, const CXCursor &cursor) {
                method_handler_base::method_visit_start(cxt, cursor, true);
            }

            void ctor_method_handler::visit_end(visitor_context &cxt, const CXCursor &cursor) {
                method_handler_base::method_visit_end(cxt, cursor, true);
            }

            void simple_method_handler::visit_start(visitor_context &cxt, const CXCursor &cursor) {
                method_handler_base::method_visit_start(cxt, cursor, false);
            }

            void simple_method_handler::visit_end(visitor_context &cxt, const CXCursor &cursor) {
                method_handler_base::method_visit_end(cxt, cursor, true);
            }

            // struct definition handler
            void struct_def_base_handler::struct_visit_start(visitor_context &cxt, const CXCursor &cursor,
                                                             struct_type type) {
                string name = clang::get_spelling(cursor);

                CXType clang_type = clang_getCursorType(cursor);
                string clang_type_name = clang::get_spelling(clang_type);

                // backup active struct and dependencies
                auto backup_struct = cxt.active_struct;

                // create empty and swap
                auto backup_dependencies = map<string, dependency>();
                swap(backup_dependencies, cxt.active_dependencies);

                // create and fill struct info
                cxt.active_struct = get_or_make_struct(cxt.symbols, clang_type_name);
                cxt.active_struct->is_visited = true;
                cxt.active_struct->name = name;
                cxt.active_struct->native_name = clang_type_name;
                cxt.active_struct->namespace_name = cxt.active_namespace;
                cxt.active_struct->type = type;
                cxt.active_struct->location = clang::get_location(cursor);
            }

            void
            struct_def_base_handler::struct_visit_end(visitor_context &cxt, const CXCursor &cursor, struct_type type) {
                CX_CXXAccessSpecifier access = clang_getCXXAccessSpecifier(cursor);

                visibility_type vis = visibility_type::visibility_private;

                if (access == CX_CXXPublic)
                    vis = visibility_type::visibility_public;

                auto native_type = clang_getCursorType(cursor);
                auto native_name = clang::get_spelling(native_type);

                cxt.active_struct->base_structs.push_back(get_or_make_struct(cxt.symbols, native_name));
            }

            void struct_def_handler::visit_start(visitor_context &cxt, const CXCursor &cursor) {
                struct_def_base_handler::struct_visit_start(cxt, cursor, struct_type::struct_type_struct);
            }

            void struct_def_handler::visit_end(visitor_context &cxt, const CXCursor &cursor) {
                struct_def_base_handler::struct_visit_end(cxt, cursor, struct_type::struct_type_struct);
            }

            void class_def_handler::visit_start(visitor_context &cxt, const CXCursor &cursor) {
                struct_def_base_handler::struct_visit_start(cxt, cursor, struct_type::struct_type_class);
            }

            void class_def_handler::visit_end(visitor_context &cxt, const CXCursor &cursor) {
                struct_def_base_handler::struct_visit_end(cxt, cursor, struct_type::struct_type_class);
            }

            void struct_base_def_handler::visit_start(visitor_context &cxt, const CXCursor &cursor) {
                CX_CXXAccessSpecifier access = clang_getCXXAccessSpecifier(cursor);

                visibility_type vis = visibility_type::visibility_private;

                if (access == CX_CXXPublic)
                    vis = visibility_type::visibility_public;

                auto native_type = clang_getCursorType(cursor);
                auto native_name = clang::get_spelling(native_type);

                cxt.active_struct->base_structs.push_back (get_or_make_struct (cxt.symbols, native_name));
            }

            lookup::handler_map_t lookup::init_map() {
                handler_map_t m;

                m[CXCursorKind::CXCursor_ClassDecl]         = make_unique < class_def_handler >();
                m[CXCursorKind::CXCursor_StructDecl]        = make_unique < struct_def_handler > ();
                m[CXCursorKind::CXCursor_CXXBaseSpecifier]  = make_unique < struct_base_def_handler > ();
                m[CXCursorKind::CXCursor_CXXMethod]         = make_unique < simple_method_handler > ();
                m[CXCursorKind::CXCursor_FunctionDecl]      = make_unique < simple_method_handler > ();
                m[CXCursorKind::CXCursor_ParmDecl]          = make_unique < param_handler > ();
                m[CXCursorKind::CXCursor_Constructor]       = make_unique < ctor_method_handler > ();
                m[CXCursorKind::CXCursor_FieldDecl]         = make_unique < field_handler > ();
                m[CXCursorKind::CXCursor_Namespace]         = make_unique < namespace_handler > ();

                return m;
            }

            lookup::lookup() : _handlers (init_map())
            {}

            child_handler * lookup::get(CXCursorKind kind) {
                static lookup * inst = new lookup ();

                auto it = inst->_handlers.find (kind);

                if (it != inst->_handlers.end ()) {
                    return it->second.get();
                } else {
                    return inst->_null_handler.get();
                }
            }

        }
    }
}