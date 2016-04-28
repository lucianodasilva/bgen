#pragma once
#ifndef _bgen_casa_generation_h_
#define _bgen_casa_generation_h_

#include <bgen_core.h>
#include <string>
#include <cinttypes>

using namespace std;

namespace bgen {
    namespace casa {
        
        namespace gen {

            enum struct js_type : uint32_t {
                unknown,
                js_int,
                js_float,
                boolean,
                array,
                object
            };

            js_type cast_type_to_js ( const bgen::type_info::shared & type );

            bool check_types_support ( const method_info & method );
            
            string namespace_to_string (const namespace_info & info, const string & separator);
            
            string namespace_to_uri (const namespace_info & info);
            
            string namespace_to_listener (const namespace_info & info);

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