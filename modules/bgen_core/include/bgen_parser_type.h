#pragma once
#ifndef _bgen_parser_type_h_
#define _bgen_parser_type_h_

#include "bgen_parser_cursor_type.h"
#include "bgen_parser_struct.h"

namespace bgen {
    namespace parser {

        struct template_argument {
            string  identifier;
            type_id type;
        };

        struct type {
            string                  identifier;
            //template_arg_vector     template_arguments;

            type_id                 base_type;
            struct_id               base_struct;
            
            bool                    is_const;
            parser::type_kind       kind;
            uint32_t                dimentions;
        };

        using type_vector = bgen::indexed_vector < string, type >;

    }
}

#endif