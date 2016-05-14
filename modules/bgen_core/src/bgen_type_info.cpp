#include "bgen_type_info.h"

#include <clang-c/Index.h>
#include <string>

#include "bgen_struct_info.h"

using namespace std;

namespace bgen {
    
    const type_info * find_root_type (const type_info * type) {
        while (type->base_type)
            type = type->base_type;
        
        return type;
    }
    
}
