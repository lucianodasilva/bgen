#pragma once
#ifndef _bgen_struct_info_h_
#define _bgen_struct_info_h_

#include <cinttypes>
#include <memory>
#include <string>
#include <vector>

#include "bgen_dependency.h"
#include "bgen_type_info.h"

using namespace std;

namespace bgen {

	class visitor;

	using namespace_info = vector < string >;
    
    enum class visibility_type : uint32_t {
		visibility_invalid,
		visibility_private,
        visibility_protected,
        visibility_public
    };

	enum class struct_type : uint32_t {
        struct_type_unknown,
		struct_type_class,
		struct_type_struct
	};
    
    struct field_info {
    private:
        
        namespace_info      _namespace_name;
        string              _name;
        source_location     _location;
        const type_info *   _type { nullptr };
        visibility_type     _visibility { visibility_type::visibility_invalid };
        
    public:
        
        field_info () = default;
        
        inline field_info (
            const namespace_info & namespace_name_v,
            const string & name_v,
            const type_info * type_v,
            const source_location & location_v,
            visibility_type visibility_v
        ) :
            _namespace_name (namespace_name_v),
            _name (name_v),
            _location (location_v),
            _type (type_v),
            _visibility (visibility_v)
        {}
                           
        inline const namespace_info & namespace_name () const {
            return _namespace_name;
        }
        
        inline const string & name () const {
            return _name;
        }
        
        inline const type_info * type () const {
            return _type;
        }
        
        inline const source_location & location () const {
            return _location;
        }
        
        inline visibility_type visibility () const {
            return _visibility;
        }
        
    };
    
    using field_vector = vector < field_info >;
    
    struct param_info {
    private:
        string				_name;
        const type_info *   _type;
    public:
        
        param_info () = default;
        inline param_info (
            const string & name_v,
            const type_info * type_v
        ) :
            _name (name_v),
            _type(type_v)
        {}
        
        inline const string & name () const { return _name; }
        inline const type_info * type () const { return _type; }
    };
    
    using param_vector = vector < param_info >;

	struct method_info {
    private:
        
        string              _name;
        namespace_info      _namespace_name;
        source_location     _location;
        
        param_vector        _params;
        const type_info *	_return_type { nullptr };
        
        visibility_type		_visibility { visibility_type::visibility_invalid };
        
        bool				_is_virtual { false };
        bool				_is_pure { false };
        bool				_is_static { false };
        bool				_is_const { false };
        bool				_is_ctor { false };
        
    public:
        
        method_info () = default;
        
        inline method_info (
            const string & name_v,
            const namespace_info & namespace_name_v,
            const source_location & location_v,
            const param_vector & params_v,
            const type_info * return_type_v,
            visibility_type visibility_v,
            bool is_virtual_v,
            bool is_pure_v,
            bool is_static_v,
            bool is_const_v,
            bool is_ctor_v
        ) :
            _name (name_v),
            _namespace_name (namespace_name_v),
            _location (location_v),
            _params (params_v),
            _return_type (return_type_v),
            _visibility (visibility_v),
            _is_virtual (is_virtual_v),
            _is_pure (is_pure_v),
            _is_static (is_static_v),
            _is_const (is_const_v),
            _is_ctor (is_ctor_v)
        {}
        
        inline const string & name () const {
            return _name;
        }
        
        inline const namespace_info & namespace_name () const {
            return _namespace_name;
        }
        
        inline const source_location & location () const {
            return _location;
        }
        
        inline const vector < param_info > & params () const {
            return _params;
        }
        
        inline const type_info * return_type () const {
            return _return_type;
        }
        
        inline visibility_type visibility () const {
            return _visibility;
        }
        
        inline bool is_virtual () const { return _is_virtual; }
        inline bool is_pure () const { return _is_pure; }
        inline bool is_static () const { return _is_static; }
        inline bool is_const () const { return _is_const; }
        inline bool is_ctor () const { return _is_ctor; }
        
	};
    
    using method_vector = vector < method_info >;
    
    class struct_info;
    
    using struct_vector = vector < struct_info * >;

    class struct_info {
    private:

		string                  _name;
		string                  _native_name;
        namespace_info          _namespace_name;
        
		struct_vector           _base_structs;

		field_vector            _fields;
		method_vector           _methods;
        
        vector < dependency >   _dependencies;
        
        source_location         _location;
        
        bool                    _is_visited { false };
        struct_type             _type { struct_type::struct_type_unknown };

    public:
        
        struct_info () = default;
        
        struct_info (
            const string & name_v,
            const string & native_name_v,
            const namespace_info & namespace_name_v,
            const struct_vector & base_structs_v,
            const field_vector & fields_v,
            const method_vector & methods_v,
            const vector < dependency > & dependencies_v,
            const source_location & location_v,
            bool is_visited_v,
            struct_type type_v
        );
  
		const string & name() const;
        const string & native_name () const;
        
        const namespace_info & namespace_name() const;
        
        const struct_vector & base_structs () const;
        
        const field_vector & fields () const;
        const method_vector & methods () const;

		const vector < dependency > & dependencies () const;
        
        const source_location & location () const;
        
        bool is_visited () const;
        struct_type type() const;
        
    };
    
    struct_vector arrange_by_dependencies (const struct_vector & structs);
    
}

#endif