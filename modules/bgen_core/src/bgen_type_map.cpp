#include "bgen_type_map.h"

namespace bgen {

	type_map::type_map() : _global (make_shared < struct_info >())
	{}

	struct_info::shared type_map::global () const {
		return _global;
	}
    
	type_info::shared type_map::make_type(const string & native_name) {
		auto & type = _types[native_name];

		if (!type)
			type.reset(new type_info());

		return type;
	}

	struct_info::shared type_map::make_struct(const string & native_name) {
		auto & strct = _structs[native_name];

        if (!strct) {
			strct.reset(new struct_info());
            strct->_native_name = native_name;
        }

		return strct;
	}

	bool type_map::has_type(const string & native_name) const {
		return _types.find(native_name) != _types.end();
	}

	bool type_map::has_struct (const string & native_name) const {
		return _structs.find(native_name) != _structs.end();
	}

	type_info::shared type_map::get_type(const string & native_name) {
		return _types[native_name];
	}

	struct_info::shared type_map::get_struct(const string & native_name) {
		return _structs[native_name];
	}
    
    const vector < struct_info::shared > & type_map::sorted_structs() const {
        return _sorted_dependencies;
    } 
}
