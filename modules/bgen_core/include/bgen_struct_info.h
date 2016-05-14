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

	using namespace_info = vector < string >;
    
    enum struct visibility_type : uint32_t {
		visibility_invalid,
		visibility_private,
        visibility_protected,
        visibility_public
    };

	enum struct struct_type : uint32_t {
        struct_type_unknown,
		struct_type_class,
		struct_type_struct
	};
    
    struct field_info {
        namespace_info      namespace_name;
        string              name;
        source_location     location;
        const type_info *   type { nullptr };
        visibility_type     visibility { visibility_type::visibility_invalid };
    };
    
    using field_vector = vector < field_info >;
    
    struct param_info {
        string				name;
        const type_info *   type;
    };
    
    using param_vector = vector < param_info >;

	struct method_info {
        string              name;
        namespace_info      namespace_name;
        source_location     location;
        
        param_vector        params;
        const type_info *	return_type { nullptr };
        
        visibility_type		visibility { visibility_type::visibility_invalid };
        
        bool				is_virtual { false };
        bool				is_pure { false };
        bool				is_static { false };
        bool				is_const { false };
        bool				is_ctor { false };
	};
    
    using method_vector = vector < method_info >;
    
    struct struct_info;
    
    using struct_vector = vector < struct_info * >;

    struct struct_info {
		string                  name;
		string                  native_name;
        namespace_info          namespace_name;
        
		struct_vector           base_structs;

		field_vector            fields;
		method_vector           methods;
        
        vector < dependency >   dependencies;
        
        source_location         location;
        
        bool                    is_visited { false };
        struct_type             type { struct_type::struct_type_unknown };
    };
    
    struct_vector arrange_by_dependencies (const struct_vector & structs);
    
}

#endif