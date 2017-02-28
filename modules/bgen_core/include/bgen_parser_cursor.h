#pragma once
#ifndef _bgen_parser_cursor_h_
#define _bgen_parser_cursor_h_

#include "bgen_details.h"

#include <string>
#include <vector>

namespace bgen {
    namespace parser {

        struct location {
            string      file;
            uint32_t    line   {0},
                        column {0};

            inline bool is_empty () const { return file.empty(); }
        };

        enum struct visibility : uint32_t {
            invalid,
            v_private,
            v_protected,
            v_public
        };

        enum struct cursor_kind : uint32_t {
            unsupported,
            decl_struct,
            decl_class,
            decl_base_specifier,
            decl_field,
            decl_method,
            decl_function,
            decl_parameter,
            decl_namespace
        };

        struct cursor_flags {
            bool 
                is_virtual : 1,
                is_pure : 1,
                is_static : 1,
                is_const : 1,
                is_ctor : 1;
        };

        struct cursor {
            parser::location    location;
            string              identifier;
            cursor_kind         kind;
        };

        using semantic_path = vector < cursor >;

    }
}

#endif