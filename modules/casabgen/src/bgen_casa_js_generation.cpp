#include "bgen_casa_js_generation.h"
#include "bgen_casa_generation.h"

using namespace std;

namespace bgen {
    namespace casa {
        namespace gen {
            namespace js {

                string map_type_cast (js_type type, const type_info::shared & native_type) {
                    switch (type) {
                        case js_type::string:
                            return "casa.details.castToString";
                        case js_type::js_int:
                            return "casa.details.castToInt";
                        case js_type::js_float:
                            return "casa.details.castToNumber";
                        case js_type::array:
                            return "casa.details.castToArray";
                        case js_type::object:
                            return native_type->struct_info ()->name () + ".fromJson";
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

                url_element::url_element(const method_info &method, bool is_last) : _method(method), _is_last (is_last){}

                void url_element::write(bgen::gen::output &out) const {
                    out.line ()
                            << _method.name () << " : " << "\"" << namespace_to_uri(_method.namespace_name()) << "/" << _method.name () << "\""
                            << (!_is_last ? "," : "");
                }

                service_get::service_get (const method_info & method, bool is_last) : _method (method), _is_last (is_last) {}

                void service_get::write (bgen::gen::output & out) const {
                    out.line () << _method.name () << " : () => {";
                    ++out.indent;

                    js_type jstype = cast_type_to_js(_method.return_type());

                    if (jstype != js_type::js_void) {
                        out.line () << "return casa.details.restGet (casa.urls." << _method.name () << ")";
                        out.line() << ".then((response) => {";

                        ++out.indent;
                        out.line() << "let value = JSON.parse(response);";

                        if (jstype == js_type::array) {
                            js_type param_js_type = cast_type_to_js(_method.return_type()->template_params()[0].type());
                            out.line()
                                << "let ret = " << map_type_cast(jstype, _method.return_type())
                                << "(value, " <<
                                map_type_cast(param_js_type, _method.return_type()->template_params()[0].type())
                                << ");";
                        } else if (jstype == js_type::object) {
                            out.line()
                                << "let ret = " << map_type_cast(jstype, _method.return_type())
                                << "(value);";
                        } else {
                            out.line()
                                << "let ret = " << map_type_cast(jstype, _method.return_type())
                                << "(value);";
                        }

                        out.line()
                        << "return ret;";

                        --out.indent;

                        out.line()
                        << "});";
                    } else {
                        out.line () << "return casa.details.restGet (casa.urls." << _method.name () << ");";
                    }

                    --out.indent;

                    out.line ()
                        << "}" << (!_is_last ? "," : "");
                }

                service_post::service_post (const method_info & method, bool is_last) : _method (method), _is_last (is_last) {}

                void service_post::write (bgen::gen::output & out) const {
                    out.line () << _method.name () << " : (param) => {";
                    ++out.indent;

                    js_type jstype = cast_type_to_js(_method.return_type());

                    if (jstype != js_type::js_void) {
                        out.line () << "return casa.details.restPost (casa.urls." << _method.name () << ", JSON.stringify(param))";
                        out.line() << ".then((response) => {";

                        ++out.indent;
                        out.line() << "let value = JSON.parse(response);";

                        if (jstype == js_type::array) {
                            js_type param_js_type = cast_type_to_js(_method.return_type()->template_params()[0].type());
                            out.line()
                                << "let ret = " << map_type_cast(jstype, _method.return_type())
                                << "(value, " <<
                                map_type_cast(param_js_type, _method.return_type()->template_params()[0].type())
                                << ");";
                        } else if (jstype == js_type::object) {
                            out.line()
                                << "let ret = " << map_type_cast(jstype, _method.return_type())
                                << "(value);";
                        } else {
                            out.line()
                                << "let ret = " << map_type_cast(jstype, _method.return_type())
                                << "(value);";
                        }

                        out.line()
                        << "return ret;";

                        --out.indent;

                        out.line()
                        << "});";
                    } else {
                        out.line () << "return casa.details.restGet (casa.urls." << _method.name () << ", JSON.stringify(param));";
                    }

                    --out.indent;

                    out.line ()
                    << "}" << (!_is_last ? "," : "");
                }

                parser_reader::parser_reader(const struct_info::shared &stct) : _struct (stct) {}

                void parser_reader::write(bgen::gen::output &out) const {
                    out.line ()
                            << _struct->name () << " = function " << _struct->name () << " () {";

                    ++out.indent;

                    for ( auto & f : _struct->fields ()) {
                        js_type jstype = cast_type_to_js(f.type ());

                        if (jstype == js_type::unknown) {
                            logger::write (f.location ()) << "\"" << f.name () << "\" has an unsuported type (field ignored)";
                            continue;
                        }

                        out.line () << "this." << f.name () << " = null;";
                    }

                    --out.indent;

                    out.line () << "};";
                    out.line ();

                    out.line () << _struct->name () << ".fromJson = (data) => {";
                    ++out.indent;
                    out.line () << "if (!data)";
                    ++out.indent;
                    out.line () << "return this";
                    --out.indent;

                    out.line () << "var obj = new " << _struct->name () << "();";

                    for ( auto & f : _struct->fields ()) {
                        js_type jstype = cast_type_to_js(f.type ());

                        if (jstype == js_type::unknown)
                            continue;

                        if (jstype == js_type::array) {
                            js_type param_js_type = cast_type_to_js (f.type()->template_params()[0].type ());
                            out.line ()
                                << "obj." << f.name () << " = " << map_type_cast (jstype, f.type())
                                << "(data ['" << f.name () << "'], " << map_type_cast (param_js_type, f.type ()) << ");";
                        } else {
                            out.line ()
                            << "obj." << f.name ()
                            << " = " << map_type_cast(jstype, f.type ())
                            << "(data['" << f.name () << "']);";
                        }
                    }

                    out.line () << "return obj;";
                    --out.indent;
                    out.line () << "};";
                }

                void generate (
                        type_map & types,
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
                    auto casa_structs = rest_client.make_item < bgen::gen::group > ();

                    // write services
                    auto & methods = types.global()->methods ();
                    size_t method_count = methods.size ();

                    for (int i = 0; i < method_count; ++i) {
                        auto & m = methods[i];
                        bool is_last = i == method_count - 1;

                        if (!check_types_support (m))
                            continue;

                        casa_urls->make_item < url_element > (m, is_last);

                        if (m.params ().size () == 0)
                            casa_services->make_item < service_get > (m, is_last);
                        else
                            casa_services->make_item < service_post > (m, is_last);
                    }

                    // write structures
                    auto source_types = types.sorted_structs();

                    // Generate structures
                    for (auto & s : source_types) {
                        if (s->is_visited())
                            casa_structs->make_item < parser_reader > (s);
                    }

                    rest_client.write ();
                }

            }
        }
    }
}