#include "bgen_parser_type_actions.h"

namespace bgen {
    namespace parser{
        namespace actions {

            void inplace_struct_action (context & cxt, const parser::cursor_type & parser_type, parser::type & source_type, const template_arg_vector & template_arguments) {
                
                parser::cursor  decl_cursor = cxt.driver->get_type_declaration(parser_type);
                parser::cursor_type    decl_type = cxt.driver->get_type (decl_cursor);

                // if not structure definition then bail
                if (!(decl_cursor.kind == cursor_kind::decl_struct || decl_cursor.kind == cursor_kind::decl_class))
                    return;

                source_type.kind = type_kind::type_kind_struct;
                source_type.base_struct = parser::get_or_make_struct_id (cxt.source_map, decl_type.identifier);

                parser::struct_info & base_struct = cxt.source_map.structs[source_type.base_struct];

                // extract semantic name
                semantic_path path = cxt.driver->get_semantic_path (decl_cursor);
				base_struct.name = parser::semantic_name_from_path(path);

                // extract template arguments

                /*
                if (template_arguments.size() > 0) {
                    for (const cursor_template_argument & ta : template_arguments) {
                        source_type.template_arguments.push_back (parser::template_argument {
                            ta.identifier,
                            cxt.interpreter.execute (ta.type.kind, cxt, ta.type)
                        });
                    }
                }
                */
            }

            struct type_action_context {
                parser::cursor_type    canon_type;
                parser::type_id source_type_index;
                parser::type &  source_type;
                bool            is_new;
            };

            type_action_context base_type_action (context & cxt, const parser::cursor_type & type) {
                parser::cursor_type canon_type = type;
            
                while (canon_type.kind != type_kind::type_kind_typedef)
                    canon_type = cxt.driver->get_canonical_type(canon_type);

                auto source_type_index = parser::get_or_make_struct_id (cxt.source_map, canon_type.identifier);
                bool is_new = source_type_index == parser::type_vector::nindex;

                if (is_new) {
                    auto emplace_ret = cxt.source_map.types.emplace_back(canon_type.identifier);
                    
                    source_type_index = emplace_ret.first;
                    parser::type & source_type = emplace_ret.second;

                    source_type.identifier = canon_type.identifier;
                    source_type.dimentions = canon_type.dimensions;
                    source_type.is_const = cxt.driver->is_const_qualified (canon_type);
                    source_type.kind = type.kind;

                    return {
                        canon_type,
                        source_type_index,
                        source_type,
                        true
                    };
                } else {
                    return {
                        canon_type,
                        source_type_index,
                        cxt.source_map.types[source_type_index],
                        false
                    };
                }

            }

            parser::type_id type_default_action (context & cxt, const parser::cursor_type & type) {
                return base_type_action(cxt, type).source_type_index;
            }

            parser::type_id type_reference_action (context & cxt, const parser::cursor_type & type) {
                auto type_cxt = base_type_action (cxt, type);

                if (type_cxt.is_new)
                    type_cxt.source_type.base_type = cxt.interpreter.execute (type_cxt.canon_type.kind, cxt, type_cxt.canon_type);

                return type_cxt.source_type_index;
            }

            parser::type_id type_struct_action (context & cxt, const parser::cursor_type & type) {
                auto type_cxt = base_type_action (cxt, type);

                if (type_cxt.is_new)
                    inplace_struct_action (cxt, type_cxt.canon_type, type_cxt.source_type);

                return type_cxt.source_type_index;
            }

            parser::type_id type_unhandled_action (context & cxt, const parser::cursor_type & type) {
                auto type_cxt = base_type_action (cxt, type);

                if (type_cxt.is_new) {
                    auto template_arguments = cxt.driver->get_template_arguments (type_cxt.canon_type);
                    if (template_arguments.size() > 0)
                        inplace_struct_action (cxt, type_cxt.canon_type, type_cxt.source_type, template_arguments);
                }

                return type_cxt.source_type_index;
            }

            parser::type_id type_enum_action (context & cxt, const parser::cursor_type & type) {
                auto type_cxt = base_type_action (cxt, type);
                type_cxt.source_type.kind = type_kind::type_kind_unhandled;

                return type_cxt.source_type_index;
            }

            parser::type_id type_array_action (context & cxt, const parser::cursor_type & type) {
                auto type_cxt = base_type_action (cxt, type);

                if (type_cxt.is_new)
                    type_cxt.source_type.base_type = cxt.interpreter.execute (type_cxt.canon_type.kind, cxt, type_cxt.canon_type);

                return type_cxt.source_type_index;
            }

            void set_default_type_actions (type_interpreter & interpreter) {

                interpreter.set_default_action (type_default_action);

                interpreter
                    .add_action (type_kind::type_kind_pointer, type_reference_action)
                    .add_action (type_kind::type_kind_lvalue_ref, type_reference_action)
                    .add_action (type_kind::type_kind_rvalue_ref, type_reference_action)
                    .add_action (type_kind::type_kind_struct, type_struct_action)
                    .add_action (type_kind::type_kind_unhandled, type_unhandled_action)
                    .add_action (type_kind::type_kind_enum, type_enum_action)
                    .add_action (type_kind::type_kind_constant_array, type_array_action)
                    .add_action (type_kind::type_kind_incomplete_array, type_array_action);

            }
        }
    }
}