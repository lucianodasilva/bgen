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
		struct_type_class,
		struct_type_struct
	};
    
    struct field_info {
    private:
        namespace_info 		_namespace_name;
        string				_name;
        type_info::shared	_type;
        location            _location;
        visibility_type		_visibility;
    public:
        
        inline const namespace_info & namespace_name () const {
            return _namespace_name;
        }
        
        inline const string & name () const {
            return _name;
        }
        
        inline const type_info::shared & type () const {
            return _type;
        }
        
        inline const location & location () const {
            return _location;
        }
        
        inline visibility_type visibility () const {
            return _visibility;
        }
        
        
        friend class visitor;
        friend class type_map;
        
    };
    
    struct param_info {
    private:
        string				_name;
        type_info::shared   _type;
    public:
        inline const string & name () const { return _name; }
        inline const type_info::shared & type () const { return _type; }
        
        
        friend class visitor;
        friend class type_map;
    };

	struct method_info {
    private:
        string				_name;

		type_info::shared	_return_type;
		vector < param_info > _params;

		namespace_info 		_namespace_name;
        
        location            _location;
        
        visibility_type		_visibility;
        
        bool				_is_virtual;
        bool				_is_pure;
        bool				_is_static;
        bool				_is_const;
        bool				_is_ctor;
        
    public:
        
        inline const string & name () const {
            return _name;
        }
        
        inline const type_info::shared & return_type () const {
            return _return_type;
        }
        
        inline const vector < param_info > & params () const {
            return _params;
        }
        
        inline const namespace_info & namespace_name () const {
            return _namespace_name;
        }
        
        inline const location & location () const {
            return _location;
        }
        
        inline visibility_type visibility () const {
            return _visibility;
        }
        
        inline bool is_virtual () const { return _is_virtual; }
        inline bool is_pure () const { return _is_pure; }
        inline bool is_static () const { return _is_static; }
        inline bool is_const () const { return _is_const; }
        inline bool is_ctor () const { return _is_ctor; }
        
        friend class visitor;
        friend class type_map;
	};

    struct struct_info {
	public:

		using shared = shared_ptr < struct_info >;

    private:

		string                  _name;
		string                  _native_name;

		vector < struct_info::shared >
                                _base_structs;

		vector < field_info >   _fields;
		vector < method_info >  _methods;

		namespace_info          _namespace_name;
        
        vector < dependency >   _dependencies;
        
        location                _location;
        
        bool                    _is_visited;
        struct_type             _type;

    public:
        
        bool is_visited () const;
  
		const string & name() const;
        const string & native_name () const;
        
        const vector < struct_info::shared > & base_structs () const;
        
        const vector < field_info > & fields () const;
        
        const vector < method_info > & methods () const;

		const namespace_info & namespace_name() const;

		struct_type type() const;
        
        const vector < dependency > & dependencies () const;
        
        const location & location () const;

		friend class visitor;
        friend class type_map;
        
    };

}

#endif