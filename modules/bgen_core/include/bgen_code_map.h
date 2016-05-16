#pragma once
#ifndef _bgen_code_map_h_
#define _bgen_code_map_h_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "bgen_struct_info.h"
#include "bgen_type_info.h"

using namespace std;

namespace bgen {
    
    using dependency_vector     = vector < struct_info * >;
    using struct_map            = map < string, unique_ptr < struct_info > >;
    using type_map              = map < string, unique_ptr < type_info > >;

    struct code_map {
        unique_ptr < struct_info >  global {new struct_info ()};
    
		type_map                    types;
		struct_map                  structs;
        dependency_vector           sorted_dependencies;
    };
    
    struct_info * get_or_make_struct (code_map & symbols, const string & native_name);
    type_info * get_or_make_type (code_map & symbols, const string & native_name);

}

#endif
