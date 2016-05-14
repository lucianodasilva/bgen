#pragma once
#ifndef _bgen_casa_generation_h_
#define _bgen_casa_generation_h_

#include <bgen_core.h>
#include <string>
#include <cinttypes>
#include "bgen_casa_common.h"

using namespace std;

namespace bgen {
    namespace casa {
        
        namespace gen {

            enum struct js_type : uint32_t {
                unknown,
                js_void,
                js_int,
                js_float,
                boolean,
                string,
                array,
                object
            };

            js_type cast_type_to_js ( const type_info * type );

            bool check_types_support ( const method_info & method );

            string id_to_string ( const casa::id_t & id, const string & separator);

            string id_to_uri ( const casa::id_t & id);

            string id_to_listener ( const casa::id_t & id );

            class boilerplate : public bgen::gen::element_base {
            private:
                string _file_name;
            public:
                
                boilerplate ( const string & file );
                
                virtual void write (bgen::gen::output & out) const override;
                
            };
        }
    }
}

#endif