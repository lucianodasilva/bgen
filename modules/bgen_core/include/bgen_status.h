#pragma once
#ifndef _bgen_status_h_
#define _bgen_status_h_

#include "bgen_common.h"
#include "bgen_parser_cursor.h"

#include <iostream>
#include <sstream>

using namespace std;

namespace bgen {
    
    enum struct state_type : uint32_t {
        success,
        warnings,
        failure
    };

    struct status_message_builder {
        stringstream        builder;
        ostream &           outstream;
        parser::location    location;
        bool                has_copy;
        
        template < class _t >
        inline status_message_builder & operator << (const _t & v) {
            builder << v;
            return *this;
        }
        
        inline status_message_builder (ostream & out, const parser::location & loc) :
            builder(),
            outstream(out),
            location (loc),
            has_copy (false)
        {}
        
        inline status_message_builder (status_message_builder && origin) :
            builder (move (origin.builder)),
            outstream (origin.outstream),
            location (move (origin.location)),
            has_copy (false)
        {
            origin.has_copy = true;
        }
        
        virtual ~status_message_builder() {
            if (!has_copy) {
                outstream << builder.str();
                
                if (!location.is_empty ())
                    outstream << " @" << location.file << ":" << location.line<< ":" << location.column;
                
                outstream << endl;
            }
        }
    };

    class status : no_copy {
    private:

        ostream &   _outstream;
        state_type  _current_state = state_type::success;

    public:

        inline status () : 
            _outstream (std::cout)
        {}

        inline status (ostream & out) : 
            _outstream(out)
        {}
        
        inline state_type current_state () { return _current_state; }
        
        inline status_message_builder info (const parser::location & location = {}) const {
            return {_outstream, location};
        }

        inline status_message_builder warn (const parser::location & location = {}) {
            if (_current_state != state_type::failure)
                _current_state = state_type::warnings;

            return {_outstream, location};
        }

        inline status_message_builder fail (const parser::location & location = {}) {
            _current_state = state_type::failure;
            return {_outstream, location};
        }

    };
    
}

#endif