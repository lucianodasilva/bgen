#pragma once
#ifndef _bgen_parser_type_h_
#define _bgen_parser_type_h_

namespace bgen{
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
            type_kind_constant_array,
            type_kind_incomplete_array
        };

        struct native_handle;

        struct type {
            string              name;

            native_handle *     native;
            type_kind           kind;
            uint32_t            dimention;

            bool                is_const;
        };

        using type_vector = vector < type >;
        
        struct template_parameter {
            string              name;
            native_handle *     native;
        };

        using template_parameter_vector = vector < template_parameter >;

    }
}

#endif