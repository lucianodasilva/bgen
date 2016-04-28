#include "bgen_casa_js_generation.h"
#include "bgen_casa_generation.h"

using namespace std;

namespace bgen {
    namespace casa {
        namespace gen {
            namespace js {

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

                    out.line ()
                            << _method.name () << " : () => {";

                    ++out.indent;
                    out.line () << "return casa.details.restGet (casa.urls." << _method.name () << ")";
                    out.line () << ".then((response) => {";
                    ++out.indent;
                    out.line () << "let value = JSON.parse(response);";

                    --out.indent;
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

                    // write services

                    auto & methods = types.global()->methods ();
                    size_t method_count = methods.size ();

                    for (int i = 0; i < method_count; ++i) {
                        auto & m = methods[i];

                        js_type type = cast_type_to_js(m.return_type ());

                        if (type == js_type::unknown) {
                            logger::write (m.location()) << "unsupported type. service ignored.";
                            continue;
                        }

                        casa_urls->make_item < url_element > (m, i == method_count - 1);

                        if (m.params ().size () == 0) {
                            // get
                        }

                    }

                    /*
                    for (auto & m : types.global()->methods ()) {
                        if (m.params ().size () == 0)
                            nlisteners->make_item<listener_get>(m);
                        else if (m.params ().size () == 1)
                            nlisteners->make_item<listener_post>(m);
                        else
                            logger::write(m.location ()) << "unsupported signature for rest service";
                    }
                     */

                }

            }
        }
    }
}