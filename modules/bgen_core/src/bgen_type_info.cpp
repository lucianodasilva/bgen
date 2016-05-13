#include "bgen_type_info.h"

#include <clang-c/Index.h>
#include <string>

#include "bgen_struct_info.h"

using namespace std;

namespace bgen {

	template_param_info::template_param_info (const  type_info & type_v) : _type (type_v){}
    
    const type_info & template_param_info::type () const {
        return _type;
    }
    
    type_info::type_info (
        const string &                  native_type_name_v,
        const template_param_vector &   template_params_v,
        const type_info *               base_type_v,
        const struct_info *             base_struct_v,
        bool                            is_const_v,
        type_kind                       kind_v,
        uint32_t                        dimention_v
    ) :
        _native_type_name(native_type_name_v),
        _template_params(template_params_v),
        _base_type(base_type_v),
        _base_struct(base_struct_v),
        _is_const(is_const_v),
        _kind (kind_v),
        _dimention(dimention_v)
    {}
    
    type_info::type_info (
        const string &                  native_type_name_v,
        const template_param_vector &   template_params_v,
        const type_info *               base_type_v,
        const struct_info *             base_struct_v,
        bool                            is_const_v,
        type_kind                       kind_v
    ) :
        _native_type_name (native_type_name_v),
        _template_params (template_params_v),
        _base_type (base_type_v),
        _base_struct (base_struct_v),
        _is_const (is_const_v),
        _kind (kind_v)
    {}
    
    type_info::type_info (
        const string &                  native_type_name_v,
        const type_info *               base_type_v,
        const struct_info *             base_struct_v,
        bool                            is_const_v,
        type_kind                       kind_v,
        uint32_t                        dimention_v
    ) :
        _native_type_name (native_type_name_v),
        _base_type (base_type_v),
        _base_struct (base_struct_v),
        _is_const (is_const_v),
        _kind (kind_v),
        _dimention(dimention_v)
    {}
    
    type_info::type_info (
        const string &                  native_type_name_v,
        const type_info *               base_type_v,
        const struct_info *             base_struct_v,
        bool                            is_const_v,
        type_kind                       kind_v
    ) :
        _native_type_name(native_type_name_v),
        _base_type(base_type_v),
        _base_struct(base_struct_v),
        _is_const(is_const_v),
        _kind (kind_v)
    {}
    
    const string & type_info::native_type_name () const {
        return _native_type_name;
    }
    
    const template_param_vector & type_info::template_params() const {
        return _template_params;
    }
    
    const type_info * type_info::base_type () const {
        return _base_type;
    }
    
    const struct_info * type_info::base_struct () const {
        return _base_struct;
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
    
    const type_info * find_root_type (const type_info * type) {
        while (type->base_type ())
            type = type->base_type();
        
        return type;
    }
}
