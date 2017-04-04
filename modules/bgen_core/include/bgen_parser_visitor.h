#pragma once
#ifndef _bgen_parser_visitor_h_
#define _bgen_parser_visitor_h_

#include "bgen_common.h"
#include "bgen_interpreter.h"

#include "bgen_parser_cursor.h"
#include "bgen_parser_driver.h"

#include "bgen_parser_source_map.h"

#include "bgen_status.h"

#include <vector>

namespace bgen {
    namespace parser {

        struct context;

        using cursor_interpreter = bgen::interpreter < cursor_kind, void (context &, const parser::cursor &) >;
        using type_interpreter = bgen::interpreter < type_kind, parser::type_id (context &, const parser::cursor_type &) >;

        class interpreter : 
            cursor_interpreter,
            type_interpreter
        {
            public:

            using cursor_interpreter::execute;
            using type_interpreter::execute;
        };

        struct context {
            const parser::cursor                cursor;
            const parser::driver *              driver;
            const visitor_controller &          controller;

            const parser::interpreter           interpreter;

            semantic_path                       path;
            parser::map                         source_map;

            vector < parser::struct_info >      struct_stack;

            bgen::status &                      status;
        };

        class visitor : no_copy {
        public:
            void begin_visit (context & cxt);
            void visit (context & cxt);
            void end_visit (context & cxt);
        };

    }
}

#endif