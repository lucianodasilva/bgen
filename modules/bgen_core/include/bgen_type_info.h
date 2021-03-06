#pragma once
#ifndef _bgen_type_info_h_
#define _bgen_type_info_h_

#include <string>
#include <memory>
#include <vector>

using namespace std;

namespace bgen {
    
    struct source_location {
        uint32_t    line   {0},
                    column {0};
        string      file;

        inline bool is_empty () const { return file.empty(); }
    };

	struct struct_info;
    
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

	struct type_info;

	struct template_param_info {
        type_info * type;
	};
    
    using template_param_vector = vector < template_param_info >;

	struct type_info {
		string                  native_name;
        template_param_vector   template_params;
        
        type_info *             base_type { nullptr };
        struct_info *           base_struct { nullptr };
        
        bool                    is_const { false };
        
        type_kind               kind { type_kind::type_kind_unhandled };
        uint32_t                dimention {};
	};
    
    const type_info * find_root_type (const type_info * type);

}

#endif
