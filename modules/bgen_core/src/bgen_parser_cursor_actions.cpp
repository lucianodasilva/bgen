#include "bgen_parser_cursor_actions.h"

#include "bgen_parser_visitor.h"

namespace bgen {
    namespace parser {
        namespace actions {

            inline bool is_valid_type (const parser::type & t) {
                return t.kind != type_kind::type_kind_invalid && t.kind != type_kind::type_kind_unhandled;
            }

            void cursor_default_action (context & cxt, const parser::cursor & cursor) {}

            void struct_base_action (context & cxt, const parser::cursor & cursor, parser::struct_kind kind) {
                auto emplace_ret = cxt.source_map.structs.find_or_emplace (cursor.identifier);
                auto & source_struct = emplace_ret.second;

                source_struct.name = parser::semantic_name_from_path(cxt.path);
                source_struct.identifier = cursor.identifier;
                source_struct.location = cursor.location;
                source_struct.kind = kind;

                cxt.struct_stack.push_back (source_struct);
                cxt.path.push_back (cursor);

                cxt.controller.visit_children ();

                cxt.struct_stack.pop_back ();
                cxt.path.pop_back ();
            }

            void struct_action (context & cxt, const parser::cursor & cursor) {
                struct_base_action (cxt, cursor, parser::struct_kind::struct_struct);
            }

            void class_action (context & cxt, const parser::cursor & cursor) {
                struct_base_action (cxt, cursor, parser::struct_kind::struct_class);
            }

            void base_spec_action (context & cxt, const parser::cursor & cursor) {
                auto & active_struct = cxt.struct_stack.back ();
                active_struct.parent_struct_ids.push_back (cxt.source_map.structs.find_or_emplace (cursor.identifier).first);
            }

            void field_action (context & cxt, const parser::cursor & cursor) {
                parser::cursor_type type = cxt.driver->get_type (cursor);

                parser::type_id source_type_id = cxt.interpreter.execute (
                    type.kind,
                    cxt, 
                    type
                );

                parser::type & source_type = cxt.source_map.types[source_type_id];

                if (!is_valid_type (source_type)) {
                    cxt.status.warn (cursor.location) << "invalid or unexpected cursor_type for field";
                    return;
                }

                parser::struct_info & active_struct = cxt.struct_stack.back ();

                active_struct.fields.emplace_back ();

                auto & field = active_struct.fields.back ();

                field.path = parser::semantic_name_from_path (cxt.path);
                field.location = cursor.location;
                field.identifier = cursor.identifier;
                field.type = source_type_id;
                field.visibility = cxt.driver->get_visibility (cursor);      
            }

            void method_action (context & cxt, const parser::cursor & cursor) {
            }

            void function_action (context & cxt, const parser::cursor & cursor) {
            }

            void parameter_action (context & cxt, const parser::cursor & cursor) {



			}

            void namespace_action (context & cxt, const parser::cursor & cursor) {
                cxt.path.push_back (cxt.cursor);
                cxt.controller.visit_children ();
                cxt.path.pop_back ();
            }

            void set_default_cursor_actions (parser::cursor_interpreter & interpreter) {
                // unsupported will run as default
                interpreter.set_default_action(cursor_default_action);

                interpreter
                    .add_action (cursor_kind::decl_struct, struct_action)
                    .add_action (cursor_kind::decl_class, class_action)
                    .add_action (cursor_kind::decl_base_specifier, base_spec_action)
                    .add_action (cursor_kind::decl_field, field_action)
                    .add_action (cursor_kind::decl_method, method_action)
                    .add_action (cursor_kind::decl_function, function_action)
                    .add_action (cursor_kind::decl_parameter, parameter_action)
                    .add_action (cursor_kind::decl_namespace, namespace_action);
            }

        }
    }
}