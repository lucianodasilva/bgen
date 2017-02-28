#include "bgen_parser_cursor_actions.h"

#include "bgen_parser_visitor.h"

namespace bgen {
    namespace parser {
        namespace actions {

            void cursor_default_action (context & cxt, const parser::cursor & cursor) {}

            void struct_action (context & cxt, const parser::cursor & cursor) {
            }

            void class_action (context & cxt, const parser::cursor & cursor) {
            }

            void base_spec_action (context & cxt, const parser::cursor & cursor) {

                auto & source_struct = cxt.struct_stack.back ();

                source::field_info field {
                    
                };
            }

            void field_action (context & cxt, const parser::cursor & cursor) {
                
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