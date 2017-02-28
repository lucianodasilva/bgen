#pragma once
#ifndef _bgen_parser_cursor_actions_h_
#define _bgen_parser_cursor_actions_h_

#include "bgen_parser_visitor.h"

#include <memory>
#include <unordered_map>

namespace bgen {
    namespace parser {

        struct context;

        namespace actions {

            void cursor_default_action (context & cxt, const parser::cursor & cursor);

            void struct_action (context & cxt, const parser::cursor & cursor);

            void class_action (context & cxt, const parser::cursor & cursor);

            void base_spec_action (context & cxt, const parser::cursor & cursor);

            void field_action (context & cxt, const parser::cursor & cursor);

            void method_action (context & cxt, const parser::cursor & cursor);

            void function_action (context & cxt, const parser::cursor & cursor);

            void parameter_action (context & cxt, const parser::cursor & cursor);

            void namespace_action (context & cxt, const parser::cursor & cursor);

            void set_default_cursor_actions (parser::interpreter & interpreter);

        }
    }
}

#endif