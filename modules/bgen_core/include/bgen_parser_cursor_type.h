#pragma once
#ifndef _bgen_parser_cursor_type_h_
#define _bgen_parser_cursor_type_h_

#include "bgen_common.h"
#include <string>
namespace bgen {
    namespace parser {

        enum class type_kind : uint32_t {
            type_kind_invalid,
            type_kind_unhandled,
            type_kind_void,
            type_kind_bool,
            type_kind_char,
            type_kind_uchar,
            type_kind_char16,
            type_kind_char32,
            type_kind_ushort,
            type_kind_uint,
            type_kind_ulong,
            type_kind_ulonglong,
            type_kind_char_s,
            type_kind_schar,
            type_kind_wchar,
            type_kind_short,
            type_kind_int,
            type_kind_long,
            type_kind_longlong,
            type_kind_int128,
            type_kind_float,
            type_kind_double,
            type_kind_longdouble,
            type_kind_nullptr,
            type_kind_pointer,
            type_kind_lvalue_ref,
            type_kind_rvalue_ref,
            type_kind_struct,
            type_kind_enum,
            type_kind_typedef,
            type_kind_constant_array,
            type_kind_incomplete_array
        };

        struct cursor_type {
            string      identifier;

            bool        is_const;
            type_kind   kind;
            uint32_t    dimensions;
        };

        struct cursor_template_argument {
            parser::cursor_type type;
            string              identifier;
        };

		size_t const template_arg_vector_default_size = 4;
        using template_arg_vector = small_vector < cursor_template_argument, template_arg_vector_default_size >;
    }
    
}

#endif