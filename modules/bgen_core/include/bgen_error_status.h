#pragma once
#ifndef _bgen_error_status_h_
#define _bgen_error_status_h_

#include "bgen_details.h"

namespace bgen {
    
    enum struct error_status_type : uint32_t {
        success,
        warnings,
        failure
    };

    struct error_status : no_copy {
    private:
        
        error_status ()    = default;
        error_status_type  _status;

    public:
        
        static error_status & get ();
        
        static error_status_type status ();
        
        static void warn ();
        static void fail ();

        static void reset ();
    };
    
}

#endif