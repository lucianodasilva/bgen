#pragma once
#ifndef _bgen_casa_js_generation_h_
#define _bgen_casa_js_generation_h_

#include <bgen_core.h>
#include <functional>
#include <string>
#include "bgen_casa_generation.h"
#include "bgen_casa_common.h"

using namespace std;

namespace bgen {
    namespace casa {
        namespace gen {
            namespace js {

                string map_type_cast (js_type type, const type_info::shared & native_type);

                inline string id_to_js (const casa::id_t & id) {
                    return id_to_string (id, ".");
                }

                inline string id_to_var_name (const casa::id_t & id) {
                    return id_to_string (id, "_");
                }

                class struct_definition : public bgen::gen::group {
                private:
                    string _id;
                public:
                    struct_definition (const string & id);
                    virtual void write (bgen::gen::output & out) const override;
                };

                class function_definition : public bgen::gen::group {
                private:
                    string _id;
                    vector < string > _params;
                public:
                    function_definition (const string & id, const vector < string > & params);
                    virtual void write (bgen::gen::output & out) const override;
                };

                class service_element : public bgen::gen::element_base {
                protected:
                    shared_ptr < service >  _service;
                    bool                    _is_last;
                public:
                    service_element (const shared_ptr < service > serv_inst, bool is_last);
                };

                class url_element : public service_element {
                public:
                    using service_element::service_element;
                    virtual void write (bgen::gen::output & out) const override;
                };

                class service_get : public service_element {
                public:
                    using service_element::service_element;
                    virtual void write (bgen::gen::output & out) const override;
                };

                class service_post : public service_element {
                public:
                    using service_element::service_element;
                    virtual void write (bgen::gen::output & out) const override;
                };

                class parser_reader : public bgen::gen::element_base {
                private:
                    shared_ptr < structure >  _struct;
                public:
                    parser_reader (const shared_ptr < structure > & stct);
                    virtual void write (bgen::gen::output & out) const override;
                };

                class init_element : public bgen::gen::element_base {
                private:
                    const vector < shared_ptr < service > > & _services;
                public:
                    init_element (const vector < shared_ptr < service > > & services);
                    virtual void write (bgen::gen::output & out) const override;
                };

                void generate (
                        casa::type_map & types,
                        const string & output_file_name,
                        const string & js_boilerplate_location
                );

            }
        }
    }
}

#endif