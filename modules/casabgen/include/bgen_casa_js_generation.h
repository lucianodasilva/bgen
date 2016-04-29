#pragma once
#ifndef _bgen_casa_js_generation_h_
#define _bgen_casa_js_generation_h_

#include <bgen_core.h>
#include <functional>
#include <string>
#include "bgen_casa_generation.h"

using namespace std;

namespace bgen {
    namespace casa {
        namespace gen {
            namespace js {

                string map_type_cast (js_type type, const type_info::shared & native_type);

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

                class url_element : public bgen::gen::element_base {
                private:
                    const method_info & _method;
                    const bool _is_last;
                public:
                    url_element (const method_info & method, bool is_last);
                    virtual void write (bgen::gen::output & out) const override;
                };

                class service_get : public bgen::gen::element_base {
                private:
                    const method_info & _method;
                    const bool _is_last;
                public:
                    service_get (const method_info & method, bool is_last);
                    virtual void write (bgen::gen::output & out) const override;
                };

                class service_post : public bgen::gen::element_base {
                private:
                    const method_info & _method;
                    const bool _is_last;
                public:
                    service_post (const method_info & method, bool is_last);
                    virtual void write (bgen::gen::output & out) const override;
                };

                class parser_reader : public bgen::gen::element_base {
                private:
                    struct_info::shared _struct;
                public:
                    parser_reader (const struct_info::shared & stct);
                    virtual void write (bgen::gen::output & out) const override;
                };

                void generate (
                        type_map & types,
                        const string & output_file_name,
                        const string & js_boilerplate_location
                );

            }
        }
    }
}

#endif