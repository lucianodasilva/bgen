#pragma once
#ifndef _bgen_source_struct_info_h_
#define _bgen_source_struct_info_h_

#include "bgen_source_dependency.h"
#include "bgen_source_info.h"
#include "bgen_source_element.h"

using namespace std;

namespace bgen {
    namespace source {

        using namespace_info = vector < string >;
        
        struct field_info {
            bgen::source::location  location;
            namespace_info          namespace_name;
            string                  name;
            size_t                  type_id { type_vector::nindex };
            visibility_type         visibility { visibility_type::visibility_invalid };
        };
        
        using field_vector = vector < field_info >;
        
        struct param_info {
            string name;
            size_t type_id { type_vector::nindex };
        };
        
        using param_vector = vector < param_info >;

        struct method_info {
            bgen::source::location      
                                location;
            string              name;
            namespace_info      namespace_name;
            
            param_vector        params;
            size_t              return_type_id { type_vector::nindex };
            
            visibility_type		visibility { visibility_type::visibility_invalid };
            
            method_flags        flags;
        };
        
        using method_vector = vector < method_info >;
        
        struct struct_info;
        
        using struct_vector = indexed_vector < string, struct_info>;

        struct struct_info {
            string                  name;
            string                  native_name;
            namespace_info          namespace_name;
            
            struct_vector           base_structs;

            field_vector            fields;
            method_vector           methods;
            
            vector < dependency >   dependencies;
            
            location                location;
            
            bool                    is_visited { false };
            struct_type             type { struct_type::struct_type_unknown };
        };

        struct_vector arrange_by_dependencies (const struct_vector & structs);
    
    }
}

#endif