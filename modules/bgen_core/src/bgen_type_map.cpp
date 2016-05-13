#include "bgen_code_map.h"

namespace bgen {

	code_map::code_map() : _global (new struct_info ())
	{}

	const struct_info & code_map::global () const {
		return *_global;
	}

    const type_map & code_map::types () const {
        return _types;
    }
    
    const struct_map & code_map::structs () const {
        return _structs;
    }
    
    
    const dependency_vector & code_map::sorted_dependencies () const {
        return _sorted_dependencies;
    } 
}
