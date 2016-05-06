#include "bgen_casa_js_generation.h"
#include "bgen_casa_generation.h"

using namespace std;

namespace bgen {
    namespace casa {
        namespace gen {
            namespace js {

                string map_type_cast (const shared_ptr < type > & ctype) {
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
                            return ctype->structure->id.name + ".fromJson";
                        default:
                            return "";
                    }
                }

                struct_definition::struct_definition(const string & id) : _id (id) {}

                void struct_definition::write(bgen::gen::output &out) const {
                    out.line ()
                            << _id << "={";
                    ++out.indent;
                    bgen::gen::group::write (out);
                    --out.indent;
                    out.line ()
                            << "};";
                }

                function_definition::function_definition (const string & id, const vector < string > & params) : _id (id), _params (params) {}

                void function_definition::write (bgen::gen::output & out) const {
                    stringstream param_list;

                    if (_params.size () > 1)
                        param_list << _params[0];

                    for (int i = 1; i < _params.size (); ++i)
                        param_list << ", " << _params[i];

                    out.line ()
                            << _id << "(" << param_list.str () << ")=>{";
                    ++out.indent;

                    bgen::gen::group::write (out);

                    --out.indent;
                    out.line ()
                            << "};";
                }

                service_element::service_element(const shared_ptr<service> serv_inst, bool is_last) :
                    _service (serv_inst),
                    _is_last (is_last)
                {}

                void url_element::write(bgen::gen::output &out) const {
                    out.line ()
                            << id_to_var_name (_service->id) << " : " << "\""
                            << id_to_uri (_service->id)
                            << "\""
                            << (!_is_last ? "," : "");
                }

                void service_get::write (bgen::gen::output & out) const {
                    out.line () << id_to_js (_service->id) << " : () => {";
                    ++out.indent;

                    casa::js_type ret_js_type = _service->return_type->js;

                    if (ret_js_type == casa::js_type::js_void) {
                        out.line () << "return casa.details.restGet (casa.urls." << id_to_var_name (_service->id) << ");";
                    } else {
                        out.line () << "return casa.details.restGet (casa.urls." << id_to_var_name (_service->id) << ")";
                        out.line() << ".then((response) => {";

                        ++out.indent;
                        out.line() << "let value = JSON.parse(response);";

                        if (ret_js_type == casa::js_type::array) {
                            out.line()
                                << "let ret = " << map_type_cast(_service->return_type)
                                << "(value, " << map_type_cast (_service->return_type->array_type)
                                << ");";
                        } else {
                            out.line()
                                << "let ret = " << map_type_cast(_service->return_type)
                                << "(value);";
                        }

                        out.line()
                        << "return ret;";
                        --out.indent;
                        out.line()
                        << "});";
                    }

                    --out.indent;
                    out.line ()
                        << "}" << (!_is_last ? "," : "");
                }

                void service_post::write (bgen::gen::output & out) const {
                    out.line () << id_to_js (_service->id) << " : (param) => {";
                    ++out.indent;

                    casa::js_type ret_js_type = _service->return_type->js;

                    if (ret_js_type == casa::js_type::js_void) {
                        out.line() << "return casa.details.restPost (casa.urls." << id_to_var_name (_service->id) <<
                        ", JSON.stringify(param));";
                    } else {
                        out.line () << "return casa.details.restPost (casa.urls." << id_to_var_name (_service->id) << ", JSON.stringify(param))";
                        out.line() << ".then((response) => {";

                        ++out.indent;
                        out.line() << "let value = JSON.parse(response);";

                        if (ret_js_type == casa::js_type::array) {
                            out.line()
                                << "let ret = " << map_type_cast(_service->return_type)
                                << "(value, " << map_type_cast(_service->return_type->array_type)
                                << ");";
                        } else {
                            out.line()
                                << "let ret = " << map_type_cast(_service->return_type)
                                << "(value);";
                        }

                        out.line()
                        << "return ret;";
                        --out.indent;
                        out.line()
                        << "});";
                    }

                    --out.indent;
                    out.line ()
                    << "}" << (!_is_last ? "," : "");
                }

                parser_reader::parser_reader(const shared_ptr<structure> & stct) : _struct (stct) {}

                void parser_reader::write(bgen::gen::output &out) const {
                    string js_name = id_to_js (_struct->id);

                    out.line ()
                            << js_name << " = function " << js_name << " () {";

                    ++out.indent;

                    for ( auto & f : _struct->fields)
                        out.line () << "this." << f.name << " = null;";

                    --out.indent;

                    out.line () << "};";
                    out.line () << id_to_js (_struct->id) << ".fromJson = (data) => {";
                    ++out.indent;
                    out.line () << "if (!data)";
                    ++out.indent;
                    out.line () << "return this";
                    --out.indent;

                    out.line () << "var obj = new " << js_name << "();";

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

                void init_element::write(bgen::gen::output &out) const {
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
                        casa::type_map & types,
                        const string & output_file_name,
                        const string & js_boilerplate_location
                ) {

                    auto & params = parameters::get ();

                    // output file
                    string rest_client_file = system::merge_path (params.host_dest, output_file_name);

                    bgen::gen::file rest_client = { rest_client_file };

                    rest_client.make_item < bgen::gen::text > ("'using strict';");

                    // make boilerplate location
                    string boilerplate_file = system::merge_path (system::get_executable_dir(), js_boilerplate_location);
                    rest_client.make_item < casa::gen::boilerplate > (boilerplate_file);

                    auto casa_urls = rest_client.make_item < struct_definition > ("casa.urls");
                    auto casa_services = rest_client.make_item < struct_definition > ("casa.services");

                    rest_client.make_item < init_element > (types.services);

                    auto casa_structs = rest_client.make_item < bgen::gen::group > ();

                    // write services
                    auto & services = types.services;
                    size_t service_count = services.size ();

                    for (int i = 0; i < service_count; ++i) {
                        auto & m = services[i];
                        bool is_last = i == service_count - 1;

                        casa_urls->make_item < url_element > (m, is_last);

                        if (m->param_type)
                            casa_services->make_item < service_post > (m, is_last);
                        else
                            casa_services->make_item < service_get > (m, is_last);
                    }

                    // Generate structures
                    for (auto & s : types.structures) {
                        casa_structs->make_item < parser_reader > (s.second);
                    }

                    rest_client.write ();
                }
            }
        }
    }
}