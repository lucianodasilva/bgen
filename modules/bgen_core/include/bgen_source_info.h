#pragma once
#ifndef _bgen_source_info_h_
#define _bgen_source_info_h_

#include "bgen_details.h"

#include <algorithm>
#include <string>
#include <vector>

using namespace std;

namespace bgen {
    namespace source {

        struct location {
            string file;

            uint32_t 
                column,
                line;

            inline bool is_empty () const { return file.empty(); }
        };

        enum struct visibility_type : uint32_t {
            visibility_invalid,
            visibility_private,
            visibility_protected,
            visibility_public
        };

        enum struct struct_type : uint32_t {
            struct_type_unknown,
            struct_type_class,
            struct_type_struct
        };

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

        struct index_t {
            int32_t id;

            static const index_t empty;
            inline bool is_empty () const { return id == empty.id; } 
        };

        const index_t index_t::empty = { -1 };

        using type_id_t = index_t;

        struct template_param_info {
            type_id_t type_id;
        };

        using template_param_vector = vector < template_param_info >;

        using struct_id_t = index_t;

        struct type {
            string                  name;
            template_param_vector   template_params;

            type_id_t               base_type_id { type_id_t::empty };
            struct_id_t             base_struct { struct_id_t::empty };

            bool                    is_const { false };

            type_kind               kind { type_kind::type_kind_unhandled };
            uint32_t                dimention {};
        };

        using type_vector = vector < type >;

        inline type_id_t find_type (const type_vector & types, const string & type_name) {
            size_t count = types.size();

            for(size_t i = 0; i < count; ++i) {
                if (types[i].name == type_name)
                    return (type_id_t {static_cast < int32_t > (i)});
            }

            return type_id_t::empty;
        }

    }
}

#endif