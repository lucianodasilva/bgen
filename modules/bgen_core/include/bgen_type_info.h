#pragma once
#ifndef _bgen_type_info_h_
#define _bgen_type_info_h_

#include <string>
#include <memory>
#include <vector>

using namespace std;

namespace bgen {
    
    class source_location {
    private:
        uint32_t    _line   {0},
                    _column {0};
        string      _file;
    public:
        inline uint32_t        line () const   { return _line; }
        inline uint32_t        column () const { return _column; }
        inline const string &  file () const   { return _file; }
        
        source_location () = default;
        source_location (uint32_t line_v, uint32_t column_v, const string & file_v) :
            _line (line_v),
            _column (column_v),
            _file (file_v)
        {}
        
        inline bool is_empty () const { return _file.empty(); }
    };

	class struct_info;
	class visitor;
    
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

	class type_info;

	class template_param_info {
	private:
		const type_info & _type;
	public:
        
		explicit template_param_info (const type_info & type_v);
        
		const type_info & type () const;
	};
    
    using template_param_vector = vector < template_param_info >;

	class type_info {
	private:
        
		string                  _native_type_name;
        template_param_vector   _template_params;
        
        const type_info *       _base_type { nullptr };
        const struct_info *     _base_struct { nullptr };
        
        bool                    _is_const { false };
        
        type_kind               _kind { type_kind::type_kind_unhandled };
        uint32_t                _dimention {};

	public:
        
        type_info () = default;
        
        type_info (
            const string &                  native_type_name_v,
            const template_param_vector &   template_params_v,
            const type_info *               base_type_v,
            const struct_info *             base_struct_v,
            bool                            is_const_v,
            type_kind                       kind_v,
            uint32_t                        dimention_v
        );
        
        type_info (
            const string &                  native_type_name_v,
            const template_param_vector &   template_params_v,
            const type_info *               base_type_v,
            const struct_info *             base_struct_v,
            bool                            is_const_v,
            type_kind                       kind_v
        );
        
        type_info (
            const string &                  native_type_name_v,
            const type_info *               base_type_v,
            const struct_info *             base_struct_v,
            bool                            is_const_v,
            type_kind                       kind_v,
            uint32_t                        dimention_v
        );
        
        type_info (
            const string &                  native_type_name_v,
            const type_info *               base_type_v,
            const struct_info *             base_struct_v,
            bool                            is_const_v,
            type_kind                       kind_v
        );
        
        const string &                  native_type_name () const;
        const template_param_vector &   template_params () const;
        const type_info *               base_type () const;
        const struct_info *             base_struct () const;
        bool                            is_const () const;
        type_kind                       kind () const;
        uint32_t                        dimention () const;
        
		friend class visitor;

	};
    
    const type_info * find_root_type (const type_info * type);

}

#endif
