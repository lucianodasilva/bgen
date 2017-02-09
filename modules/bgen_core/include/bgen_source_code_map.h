#pragma once
#ifndef _bgen_source_code_map_h_
#define _bgen_source_code_map_h_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "bgen_source_info.h"
#include "bgen_source_struct_info.h"

using namespace std;

namespace bgen {
    namespace source {
        struct code_map {
            source::struct_info  global {};
        
            type_vector                 types;
            struct_vector               structs;

            dependency_vector           sorted_dependencies;
        };
    }
}

#endif
