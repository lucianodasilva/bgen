#include "bgen_casa_js_generation.h"
#include "bgen_casa_generation.h"

using namespace std;

namespace bgen {
    namespace casa {
        namespace gen {
            namespace js {

                string map_type_cast (const shared_ptr < simple_type > & ctype) {
                    switch (ctype->js) {
                        case casa::js_type::string:
                            return "casa.details.castToString";
                        case casa::js_type::js_int:
                            return "casa.details.castToInt";
                        case casa::js_type::js_float:
                            return "casa.details.castToNumber";
                        case casa::js_type::array:
                            return "casa.details.castToArray";
                        case casa::js_type::object:
                            return "casa.details." + id_to_var_name (ctype->structure->id) + "_fromJson";
                        default:
                            return "";
                    }
                }
                
                struct_definition::struct_definition(const string & id) : _id (id) {}

                void struct_definition::write(context & cxt, bgen::gen::output &out) const {
                    out.line ()
                            << _id << "={";
                    ++out.indent;
                    bgen::gen::group::write (cxt, out);
                    --out.indent;
                    out.line ()
                            << "};";
                }
                
                nspace_node::nspace_node (const string & name_v) : name (name_v) {}

                nspace_node::shared inline_definition::get_parent (const id_t & id) {
                    nspace_node::shared parent = _root;

                    for (auto & n : id.nspace) {
                        bool found = false;

                        for (auto & pn : parent->nodes) {
                            if (pn->name == n) {
                                parent = pn;
                                found = true;
                                break;
                            }
                        }

                        if (!found) {
                            auto np = make_shared < nspace_node > (n);
                            parent->nodes.push_back(np);
                            parent = np;
                        }
                    }

                    return parent;
                }
                
                void inline_definition::write_struct (bgen::gen::output & out, const shared_ptr < simple_struct > & strt, bool is_last) const {
                    string js_name = id_to_var_name (strt->id);
                    
                    out.line ()
                            << strt->id.name << " : function " << js_name << " () {";

                    ++out.indent;

                    for ( auto & f : strt->fields)
                        out.line () << "this." << f.name << " = null;";

                    --out.indent;
                    out.line () << "}"
                                << (is_last ? "" : ",");
                }
                
                void inline_definition::write_service (bgen::gen::output & out, const shared_ptr < service > & serv, bool is_last) const {
                    string js_name = id_to_var_name (serv->id);

                    out.line () << serv->id.name
                                << " : function " << js_name << "("
                                << ( serv->param_type ? "param, " : "")
                                << "resolve, reject) {";
                    ++out.indent;

                    casa::js_type ret_js_type = serv->return_type->js;

                    if (ret_js_type == casa::js_type::js_void) {
                        if (serv->param_type) 
                            out.line () << "casa.details.restPost (casa.urls." << id_to_var_name (serv->id)
                                        << ", JSON.stringify(param), ";
                        else
                            out.line () << "casa.details.restGet (casa.urls." << id_to_var_name (serv->id) << ", ";

                        ++out.indent;
                        out.line () << "() => { if (resolve != null) resolve (); },";
                        out.line () << "reject";
                        --out.indent;

                        out.line () << ");";

                    } else {
                        if (serv->param_type)
                            out.line () << "casa.details.restPost (casa.urls." << id_to_var_name (serv->id)
                                        << ", JSON.stringify(param), ";
                        else
                            out.line () << "casa.details.restGet (casa.urls." << id_to_var_name (serv->id) << ", ";

                        ++out.indent;
                        out.line() << "(response) => {";

                        ++out.indent;
                        out.line() << "let value = JSON.parse(response);";

                        if (ret_js_type == casa::js_type::array) {
                            out.line()
                                << "let ret = " << map_type_cast(serv->return_type)
                                << "(value, " << map_type_cast (serv->return_type->array_type)
                                << ");";
                        } else {
                            out.line()
                                << "let ret = " << map_type_cast(serv->return_type)
                                << "(value);";
                        }

                        out.line () << "if (resolve != null) resolve (ret);";

                        --out.indent;
                        out.line() << "}, reject";
                        --out.indent;
                        out.line () << ");";
                    }

                    --out.indent;
                    out.line () << "}"
                                << (is_last ? "" : ",");
                }
                
                void inline_definition::write_node_children (bgen::gen::output & out, const shared_ptr < nspace_node > & n) const {
                    
                    // write child nodes
                    if (n->nodes.size () > 0) {
                        bool is_last_group = 
                            n->structures.size () == 0 &&
                            n->services.size () == 0;
                        
                        auto & last_node = *(end (n->nodes) - 1);

                        for (auto & cn : n->nodes) {
                            bool is_last = 
                                (cn == last_node) && 
                                is_last_group; 
                                
                            write_node (out, cn, is_last);
                        }
                    }
                    
                    // write structure proxys
                    if (n->structures.size () > 0) {
                        bool is_last_group = n->services.size () == 0;
                        
                        auto & last_struct = *(end (n->structures) - 1);
                        
                        for (auto & sn : n->structures) {
                            bool is_last = 
                                (sn == last_struct) &&
                                is_last_group;
                                
                            write_struct (out, sn, is_last);
                        }
                    }
                    
                    // write service proxys
                    if (n->services.size () > 0) {
                        auto & last_service = *(end (n->services) - 1);
                        
                        for (auto & serv_n : n->services) {
                            bool is_last =
                                serv_n == last_service;
                            
                            write_service (out, serv_n, is_last);
                        }
                    }
                }
                
                void inline_definition::write_node (bgen::gen::output & out, const shared_ptr < nspace_node > & n, bool is_last) const {
                    out.line () << n->name << " : {";
                    ++out.indent;
                    
                    write_node_children(out, n);
                    
                    --out.indent;
                    out.line () << "}" << 
                        (is_last ? "," : "");
                }
                
                void inline_definition::add (const shared_ptr < casa::simple_struct > & stct) {
                    auto parent = get_parent (stct->id);
                    parent->structures.push_back (stct);
                }

                void inline_definition::add (const shared_ptr < casa::service > & serv) {
                    auto parent = get_parent (serv->id);
                    parent->services.push_back (serv);
                }

                inline_definition::inline_definition() : _root(make_shared < nspace_node > ("")) { }

                void inline_definition::write(context & cxt, bgen::gen::output &out) const {
                    // write root namespaces
                    
                    for (auto & rnode : _root->nodes) {
                        out.line ()
                            << "var " << rnode->name << "={";
                        ++out.indent;
                        
                        // handle children
                        write_node_children (out, rnode);
                        
                        --out.indent;
                        out.line ()
                            << "};";
                    }
                }
                
                url_element::url_element (const shared_ptr < service > & serv, bool is_last)
                    : _service (serv), _is_last (is_last) {}

                void url_element::write(context & cxt, bgen::gen::output &out) const {
                    out.line ()
                            << id_to_var_name (_service->id) << " : " << "\""
                            << id_to_uri (_service->id)
                            << "\""
                            << (!_is_last ? "," : "");
                }

                parser_reader::parser_reader(const shared_ptr<simple_struct> & stct) : _struct (stct) {}

                void parser_reader::write(context & cxt, bgen::gen::output &out) const {
                    string js_name = id_to_var_name (_struct->id) + "_fromJson";
                    string details_name = "casa.details." + js_name;

                    out.line ()
                        << details_name << " = function " << js_name << " (data) {";

                    ++out.indent;
                    out.line () << "if (!data)";
                    ++out.indent;
                    out.line () << "return null";
                    --out.indent;

                    out.line () << "var obj = new " << id_to_js (_struct->id) << "();";

                    for ( auto & f : _struct->fields) {
                        casa::js_type f_type = f.type->js;

                        if (f_type == casa::js_type::array) {
                            out.line ()
                                << "obj." << f.name << " = " << map_type_cast (f.type)
                                << "(data ['" << f.name << "'], " << map_type_cast (f.type->array_type) << ");";
                        } else {
                            out.line ()
                            << "obj." << f.name
                            << " = " << map_type_cast(f.type)
                            << "(data['" << f.name << "']);";
                        }
                    }

                    out.line () << "return obj;";
                    --out.indent;
                    out.line () << "};";
                }

                init_element::init_element(const vector<shared_ptr<service> > &services) : _services (services){}

                void init_element::write(context & cxt, bgen::gen::output &out) const {
                    out.line ()
                            << "casa.init = function (url_base) {";
                    ++out.indent;

                    for (auto & s : _services) {
                        string var_name = id_to_var_name (s->id);

                        out.line ()
                                << "casa.urls." << var_name << " = url_base + casa.urls." << var_name << ";";
                    }

                    --out.indent;
                    out.line ()
                            << "};";
                }

                void generate (
                        context & cxt,
                        casa::type_map & types,
                        const string & output_file_name,
                        const string & js_boilerplate_location
                ) {
                    // output file
                    string rest_client_file = system::merge_path (cxt.parameters.host_dest, output_file_name);

                    bgen::gen::file rest_client = { rest_client_file };
                    
                    // make boilerplate location
                    string boilerplate_file = system::merge_path (system::get_executable_dir(), js_boilerplate_location);
                    rest_client.make_item < casa::gen::boilerplate > (boilerplate_file);

                    auto casa_urls = rest_client.make_item < struct_definition > ("casa.urls");
                    auto casa_parsers = rest_client.make_item < bgen::gen::group > ();

                    rest_client.make_item < init_element > (types.services);

                    auto proxies = rest_client.make_item < inline_definition > ();

                    // write services
                    auto & services = types.services;
                    size_t service_count = services.size ();

                    for (int i = 0; i < service_count; ++i) {
                        auto & m = services[i];
                        bool is_last = i == service_count - 1;

                        casa_urls->make_item < url_element > (m, is_last);
                        proxies->add (m);
                    }

                    // Generate structures
                    for (auto & s : types.structures) {
                        casa_parsers->make_item < parser_reader > (s.second);
                        proxies->add (s.second);
                    }

                    rest_client.write (cxt);
                }
            }
        }
    }
}