#pragma once
#ifndef _bgen_status_h_
#define _bgen_status_h_

#include "bgen_details.h"
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
        parser::location  location;
        bool                    has_copy;
        stringstream            builder;
        
        template < class _t >
        inline status_message_builder & operator << (const _t & v) {
            builder << v;
            return *this;
        }
        
        inline status_message_builder (const parser::location & loc) :
            location (loc),
            has_copy (false)
        {}
        
        inline status_message_builder (status_message_builder && origin) :
            location (move (origin.location)),
            has_copy (false),
            builder (move (origin.builder))
        {
            origin.has_copy = true;
        }
        
        virtual ~status_message_builder() {
            if (!has_copy) {
                cout << builder.str();
                
                if (!location.is_empty ())
                    cout << " @" << location.file << ":" << location.line<< ":" << location.column;
                
                cout << endl;
            }
        }
    };

    class status : no_copy {
    private:

        state_type  _current_state;

    public:

        status ()= default;
        
        inline state_type current_state () { return _current_state; }
        
        inline status_message_builder info (const parser::location & location = {}) const {
            return {location};
        }

        inline status_message_builder warn (const parser::location & location = {}) {
            if (_current_state != state_type::failure)
                _current_state = state_type::warnings;

            return {location};
        }

        inline status_message_builder fail (const parser::location & location = {}) {
            _current_state = state_type::failure;
            return {location};
        }

    };
    
}

#endif