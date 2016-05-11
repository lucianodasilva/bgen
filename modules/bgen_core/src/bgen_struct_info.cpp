#include "bgen_struct_info.h"

#include <vector>

namespace bgen {
    
    bool struct_info::is_visited() const {
        return _is_visited;
    }
    
	const string & struct_info::name() const {
		return _name;
	}

    const string & struct_info::native_name () const {
        return _native_name;
    }
    
    const vector < struct_info::shared > & struct_info::base_structs () const {
        return _base_structs;
    }
    
    const vector < field_info > & struct_info::fields () const {
        return _fields;
    }
    
    const vector < method_info > & struct_info::methods () const {
        return _methods;
    }

	const namespace_info & struct_info::namespace_name() const {
		return _namespace_name;
	}

	struct_type struct_info::type() const {
		return _type;
	}
    
    const vector < dependency > & struct_info::dependencies () const {
        return _dependencies;
    }
    
    const source_location & struct_info::location () const {
        return _location;
    }

}