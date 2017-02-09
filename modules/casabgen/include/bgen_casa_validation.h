#pragma once
#ifndef _bgen_casa_validation_h_
#define _bgen_casa_validation_h_

#include "bgen_casa_common.h"
#include <bgen_core.h>

#include "vector"

using namespace std;

namespace bgen {
    namespace casa {
        namespace validation {

            shared_ptr < simple_type > type_check (const type_info * native_type, casa::type_map & dest);

            shared_ptr < simple_struct > struct_check (const source::struct_info * src, casa::type_map & dest);

            void service_check (const source::method_info & src, casa::type_map & dest);

            void validate_and_set (const code_map & symbols, casa::type_map & dest);

        }
    }
}

#endif