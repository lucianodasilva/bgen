#include "bgen_error_status.h"

namespace bgen {
    
    error_status & error_status::get () {
        static error_status inst;
        return inst;
    }
    
    error_status_type error_status::status () {
        return get ()._status;
    }
    
    void error_status::warn () {
        auto & s = get ();
        
        if (s._status != error_status_type::failure)
            s._status = error_status_type::warnings;
    }
    
    void error_status::fail () {
        get ()._status = error_status_type::failure;
    }
}