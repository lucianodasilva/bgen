#pragma once
#ifndef _bgen_type_info_h_
#define _bgen_type_info_h_

#include <string>
#include <memory>

using namespace std;

namespace bgen {
    
    struct location {
        uint32_t	line,
        column;
        string		file;
        
        inline bool is_empty () { return file.empty(); }
    };

	struct  struct_info;
	class   visitor;
    
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

	struct type_info {
	public:

		using shared = shared_ptr < type_info >;

	private:

		string 		_native_type_name;
		bool		_is_const;;
        
        type_kind	_kind;
		uint32_t	_dimention;

		type_info::shared _base;

		shared_ptr < struct_info > _struct_info;

	public:

		const string & native_type_name () const;

		bool		is_const () const;

        type_kind	kind () const;
		uint32_t	dimention () const;

		type_info::shared base() const;

        shared_ptr < struct_info > struct_info () const;
        
        static type_info::shared find_root (type_info::shared type);

		friend class visitor;

	};

}

#endif
