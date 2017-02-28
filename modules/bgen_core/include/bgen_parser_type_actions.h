#pragma once
#ifndef _bgen_parser_type_actions_h_
#define _bgen_parser_type_actions_h_

#include "bgen_details.h"

#include "bgen_parser_type.h"
#include "bgen_source_type.h"
#include "bgen_parser_visitor.h"

namespace bgen {
    namespace parser {
        namespace actions {

            void inplace_struct_action (context & cxt, const parser::type & parser_type, source::type & source_type, const template_arg_vector & template_arguments = template_arg_vector ());

            source::type_id type_default_action (context & cxt, const parser::type & type);

            source::type_id type_reference_action (context & cxt, const parser::type & type);

            source::type_id type_struct_action (context & cxt, const parser::type & type);

            source::type_id type_unhandled_action (context & cxt, const parser::type & type);

            source::type_id type_enum_action (context & cxt, const parser::type & type);

            source::type_id type_array_action (context & cxt, const parser::type & type);

            void set_default_type_actions (parser::interpreter & interpreter);
        }
    }
}

#endif