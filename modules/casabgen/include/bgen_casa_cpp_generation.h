#pragma once
#ifndef _bgen_casa_cpp_generation_h_
#define _bgen_casa_cpp_generation_h_

#include <bgen_core.h>
#include <string>
#include "bgen_casa_common.h"
#include "bgen_casa_generation.h"

using namespace std;

namespace bgen {
    namespace casa {
        
        namespace gen {
            
            namespace cpp {

                string method_listener_name(const shared_ptr < service > & service_inst, const string &rest_method);

                inline string id_to_cpp (const casa::id_t & id) {
                    return id_to_string (id, "::");
                }

                class struct_element : public bgen::gen::element_base {
                protected:
                    shared_ptr < simple_struct > _struct;
                public:
                    struct_element (const shared_ptr < simple_struct > & stct);
                };

                class parser_reader_pre : public struct_element {
                public:
                    using struct_element::struct_element;
                    virtual void write(context & cxt, bgen::gen::output &out) const override;

                };

                class parser_reader : public struct_element {
                public:
                    using struct_element::struct_element;
                    virtual void write(context & cxt, bgen::gen::output &out) const override;
                };

                class parser_writer_pre : public struct_element {
                public:
                    using struct_element::struct_element;
                    virtual void write(context & cxt, bgen::gen::output &out) const override;
                };

                class parser_writer : public struct_element {
                public:
                    using struct_element::struct_element;
                    virtual void write(context & cxt, bgen::gen::output &out) const override;
                };

                class listener_post : public bgen::gen::element_base {
                private:
                    shared_ptr < service > _service;
                public:
                    listener_post(const shared_ptr < service > & service_inst);
                    virtual void write(context & cxt, bgen::gen::output &out) const override;
                };

                class listener_get : public bgen::gen::element_base {
                private:
                    shared_ptr < service > _service;
                public:
                    listener_get(const shared_ptr < service > & service_inst);
                    virtual void write(context & cxt, bgen::gen::output &out) const override;
                };

                class register_listeners : public bgen::gen::element_base {
                protected:
                    const vector < shared_ptr < service > > & _services;
                public:
                    register_listeners (const vector < shared_ptr < service > > & services);
                    virtual void write(context & cxt, bgen::gen::output &out) const override;
                };

                void generate (
                    casa::type_map & types,
                    const string & output_file_name,
                    const string & parser_boilerplate_location,
                    const string & listener_boilerplate_location
                );

            }
        }
    }
}

#endif