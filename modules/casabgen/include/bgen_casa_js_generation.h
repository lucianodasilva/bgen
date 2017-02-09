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

                string map_type_cast (js_type type, const type_info * native_type);

                inline string id_to_js (const casa::id_t & id) {
                    return id_to_string (id, ".");
                }

                inline string id_to_var_name (const casa::id_t & id) {
                    return id_to_string (id, "_");
                }

                struct nspace_node {
                    using shared = shared_ptr < nspace_node >;
                    using list = vector < shared >;

                    string name;
                    
                    nspace_node (const string & name_v);

                    list nodes;
                    vector < shared_ptr < service > > services;
                    vector < shared_ptr < simple_struct > > structures;
                };

                class inline_definition : public bgen::gen::element_base {
                private:

                    nspace_node::shared _root;
                    
                    nspace_node::shared get_parent ( const id_t & id );
                    
                    void write_struct (bgen::gen::output & out, const shared_ptr < simple_struct > & strt, bool is_last) const;
                    void write_service (bgen::gen::output & out, const shared_ptr < service > & serv, bool is_last) const;
                    void write_node_children (bgen::gen::output & out, const shared_ptr < nspace_node > & n) const;
                    void write_node (bgen::gen::output & out, const shared_ptr < nspace_node > & n, bool is_last) const;
                    
                public:
                
                    void add (const shared_ptr < casa::simple_struct > & stct);
                    void add (const shared_ptr < casa::service > & serv);

                    inline_definition ();
                    virtual void write (context & cxt, bgen::gen::output & out) const override;
                };

                class struct_definition : public bgen::gen::group {
                private:
                    string _id;
                public:
                    struct_definition (const string & id);
                    virtual void write (context & cxt, bgen::gen::output & out) const override;
                };

                class url_element : public bgen::gen::element_base {
                private:
                    shared_ptr < service > _service;
                    bool _is_last;
                public:
                    url_element (const shared_ptr < service > & serv, bool is_last);
                    virtual void write (context & cxt, bgen::gen::output & out) const override;
                };

                class parser_reader : public bgen::gen::element_base {
                private:
                    shared_ptr < simple_struct >  _struct;
                public:
                    parser_reader (const shared_ptr < simple_struct > & stct);
                    virtual void write (context & cxt, bgen::gen::output & out) const override;
                };

                class init_element : public bgen::gen::element_base {
                private:
                    const vector < shared_ptr < service > > & _services;
                public:
                    init_element (const vector < shared_ptr < service > > & services);
                    virtual void write (context & cxt, bgen::gen::output & out) const override;
                };

                void generate (
                        context & cxt,
                        casa::type_map & types,
                        const string & output_file_name,
                        const string & js_boilerplate_location
                );

            }
        }
    }
}

#endif