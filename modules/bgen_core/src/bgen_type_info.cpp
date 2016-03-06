#include "bgen_type_info.h"

#include <clang-c/Index.h>
#include <string>

#include "bgen_struct_info.h"

using namespace std;

namespace bgen {

	const string & type_info::native_type_name () const {
		return _native_type_name;
	}
    
	bool type_info::is_const () const {
		return _is_const;
	}

	type_kind type_info::kind() const {
		return _kind;
	}

	uint32_t type_info::dimention () const {
		return _dimention;
	}

	type_info::shared type_info::base() const {
		return _base;
	}

    shared_ptr < struct_info > type_info::struct_info() const {
        return _struct_info;
    }
    
    type_info::shared type_info::find_root (type_info::shared type) {
        while (type->base ())
            type = type->base ();
        
        return type;
    }
}
