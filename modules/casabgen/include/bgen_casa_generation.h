#pragma once
#ifndef _bgen_casa_generation_h_
#define _bgen_casa_generation_h_

#include <bgen_core.h>

#include <string>

using namespace std;

namespace bgen {
    namespace casa {
        
        namespace gen {
            
            string namespace_to_string (const namespace_info & info, const string & separator);
            
            string namespace_to_uri (const namespace_info & info);
            
            string namespace_to_listener (const namespace_info & info);
            
            string method_listener_name (const method_info & info, const string & rest_method);

            class parser_reader : public bgen::gen::element_base {
            private:
                struct_info::shared _info;
            public:
                
                parser_reader (const struct_info::shared & info);
                
                virtual void write (bgen::gen::output & out) const override;
                
            };
            
            class parser_writer : public bgen::gen::element_base {
            private:
                struct_info::shared _info;
            public:
                
                parser_writer (const struct_info::shared & info);
                
                virtual void write (bgen::gen::output & out) const override;
                
            };
            
            class listener_post : public bgen::gen::element_base {
            private:
                method_info _info;
            public:
                
                listener_post (const method_info & info);
                
                virtual void write (bgen::gen::output & out) const override;
                
            };
            
            class listener_get : public bgen::gen::element_base {
            private:
                method_info _info;
            public:
                
                listener_get (const method_info & info);
                
                virtual void write (bgen::gen::output & out) const override;
                
            };
            
            class register_listeners : public bgen::gen::element_base {
            private:
                struct_info::shared _info;
            public:
                
                register_listeners (const struct_info::shared & info);
                
                virtual void write (bgen::gen::output & out) const override;
                
            };
            
            class boilerplate : public bgen::gen::element_base {
            private:
                string _file_name;
            public:
                
                boilerplate ( const string & file );
                
                virtual void write (bgen::gen::output & out) const override;
                
            };
            
        }

        extern const string parser_boilerplate_location;
        extern const string listener_boilerplate_location;
        
    }
}

#endif