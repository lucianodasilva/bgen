#pragma once
#ifndef _bgen_context_h_
#define _bgen_context_h_

#include "bgen_parameters.h"
#include "bgen_status.h"

namespace bgen {

    struct context : no_copy {
        bgen::parameters parameters;
        bgen::status status;
    };

}

#endif