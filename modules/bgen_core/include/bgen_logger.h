#pragma once
#ifndef _bgen_logger_h_
#define _bgen_logger_h_

#include <vector>
#include <sstream>
#include <iostream>

#include "bgen_type_info.h"

using namespace std;

namespace bgen {

	namespace details {
        struct message_builder {
            bgen::source_location   location;
            bool                    has_copy;
            stringstream            builder;
            
            template < class _t >
            inline message_builder & operator << (const _t & v) {
                builder << v;
                return *this;
            }
            
            inline message_builder (const bgen::source_location & loc) :
                location (loc),
                has_copy (false)
            {}
            
            inline message_builder(message_builder && origin) :
            location (move (origin.location)),
            has_copy (false),
            builder (move (origin.builder))
            {
                origin.has_copy = true;
            }
            
            virtual ~message_builder() {
                if (!has_copy) {
                    cout << builder.str();
                    
                    if (!location.is_empty ())
                        cout << " @" << location.file << ":" << location.line<< ":" << location.column;
                    
                    cout << endl;
                }
            }
        };
	}

	struct logger {
		public:
        
        inline static details::message_builder write (const source_location & loc) {
            return details::message_builder(loc);
        }
        
        inline static details::message_builder write () {
            return details::message_builder({});
        }
        
	};

}

#endif