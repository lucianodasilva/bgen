#include "bgen_casa_cpp_generation.h"
#include "bgen_casa_generation.h"

#include <bgen_core.h>
#include <string>
#include <sstream>

using namespace std;

namespace bgen {
    namespace casa {
        
        namespace gen {

            namespace cpp {

                string method_listener_name(const shared_ptr < service > & service_inst, const string &rest_method) {
                    stringstream stream;

                    stream
                        << "handle_"
                        << gen::id_to_listener(service_inst->id)
                        << "_"
                        << rest_method;

                    return stream.str();
                }

                struct_element::struct_element(const shared_ptr<simple_struct> &stct) : _struct (stct){}

                void parser_reader_pre::write(bgen::gen::output &out) const {
                    out.line()
                    << "inline bool read (const value & source, "
                    << _struct->native_struct->native_name()
                    << " & dest );";
                }

                void parser_reader::write(bgen::gen::output &out) const {
                    out.line()
                    << "inline bool read (const value & source, "
                    << _struct->native_struct->native_name()
                    << " & dest ) {";

                    ++out.indent;

                    out.line()
                    << "if (!source.is_object()) return false;";
                    out.line()
                    << "auto source_obj = source.as_object ();";
                    out.line();
                    out.line() << "bool success = true;";

                    for (auto &f : _struct->fields) {
                        out.line() << "success = read_field_details (source_obj, \""
                        << f.name << "\", dest." << f.name <<
                        ") && success;";
                    }

                    out.line()
                    << "return success;";

                    --out.indent;

                    out.line() << "}";
                    out.line();
                }

                void parser_writer::write(bgen::gen::output &out) const {

                    out.line()
                    << "inline value write (const "
                    << _struct->native_struct->native_name()
                    << " & source ) {";

                    ++out.indent;

                    out.line()
                    << "auto object = web::json::value::object ();";

                    for (auto &f : _struct->fields) {
                        out.line()
                        << "object [\"" << f.name << "\"] = write (source."
                        << f.name << ");";
                    }

                    out.line()
                    << "return object;";

                    --out.indent;

                    out.line()
                    << "}";
                }

                void parser_writer_pre::write(bgen::gen::output &out) const {
                    out.line()
                    << "inline value write (const "
                    << _struct->native_struct->native_name()
                    << " & source );";
                }

                listener_post::listener_post(const shared_ptr < service > & service_inst) :
                        _service (service_inst) { }

                void listener_post::write(bgen::gen::output &out) const {

                    out.line()
                    << "inline void "
                    << method_listener_name(_service, "post")
                    << " (http_request request) {";

                    ++out.indent;

                    out.line()
                    << "handle_post <"
                    << _service->param_type->native_type->native_type_name() << ", "
                    << _service->return_type->native_type->native_type_name() << ", "
                    << id_to_cpp (_service->id)
                    << "> (request);";

                    --out.indent;

                    out.line()
                    << "}";
                }

                listener_get::listener_get(const shared_ptr < service > & service_inst) :
                        _service (service_inst) {}

                void listener_get::write(bgen::gen::output &out) const {

                    out.line()
                    << "inline void "
                    << method_listener_name(_service, "get")
                    << " (http_request request) {";

                    ++out.indent;

                    out.line()
                    << "handle_get <"
                    << _service->return_type->native_type->native_type_name() << ", "
                    << id_to_cpp (_service->id)
                    << "> (request);";

                    --out.indent;

                    out.line()
                    << "}";
                }

                register_listeners::register_listeners(const vector<shared_ptr<service> > &services)
                    : _services (services)
                {}

                void register_listeners::write(bgen::gen::output &out) const {

                    out.line() << "inline std::vector < http_listener > register_listeners (";
                    ++out.indent;
                    out.line() << "const std::string & address, ";
                    out.line() << "http_listener_config config = http_listener_config ()";
                    --out.indent;
                    out.line() << ") {";

                    ++out.indent;

                    out.line()
                    << "std::vector < http_listener > lv;";

                    out.line()
                    << "lv.reserve (" << _services.size() << ");";

                    out.line()
                    << "uri_builder builder (address);";

                    int i = 0;

                    for (auto & s: _services ) {
                        out.line()
                        << "builder.set_path(\"" << id_to_uri (s->id) << "\");";
                        out.line()
                        << "new (&lv [" << i << "]) http_listener (builder.to_uri().to_string(), config);";

                        if (!s->param_type) {
                            out.line()
                            << "lv [" << i << "].support(methods::GET, listeners::" << method_listener_name(s, "get") <<
                            ");";
                        } else {
                            out.line()
                            << "lv [" << i << "].support(methods::POST, listeners::" <<
                            method_listener_name(s, "post") <<
                            ");";
                        }

                        out.line()
                        << "lv [" << i << "].open().wait();";

                        ++i;
                    }

                    out.line()
                    << "return lv;";

                    --out.indent;

                    out.line() << "}";
                }

                void generate (
                        casa::type_map & types,
                        const string & output_file_name,
                        const string & parser_boilerplate_location,
                        const string & listener_boilerplate_location
                ) {
                    auto & params = parameters::get ();

                    // output file
                    string rest_server_header_file = system::merge_path (params.host_dest, output_file_name);

                    bgen::gen::file rest_server_header = {rest_server_header_file};

                    // output root element
                    auto root = rest_server_header.make_item< bgen::gen::cpp::header_guard >("_casa_rest_server_h_");

                    root->make_item < bgen::gen::cpp::includes > ( vector <string> {
                            "cpprest/json.h",
                            "cpprest/http_listener.h",
                            "cinttypes",
                            "map",
                            "memory",
                            "string",
                            "vector",
                            "type_traits"
                    });

                    root->make_item < bgen::gen::text > ("using namespace web;");
                    root->make_item < bgen::gen::text > ("using namespace web::json;");
                    root->make_item < bgen::gen::text > ("using namespace web::http;");
                    root->make_item < bgen::gen::text > ("using namespace web::http::experimental::listener;");

                    root->make_item < bgen::gen::text > ("using namespace std;");

                    auto nspace = root->make_item < bgen::gen::cpp::cpp_namespace > (
                            vector < string > {"casa", "rest"}
                    );

                    auto nspace_parse = nspace->make_item < bgen::gen::cpp::cpp_namespace > (
                            vector < string > { "parse" }
                    );

                    auto nspace_parse_readers_predef = nspace_parse->make_item < bgen::gen::group >();
                    auto nspace_parse_writers_predef = nspace_parse->make_item < bgen::gen::group >();

                    // Write boilerplate
                    string parser_bplate_file = system::merge_path(system::get_executable_dir(), parser_boilerplate_location);
                    nspace_parse->make_item<casa::gen::boilerplate>(parser_bplate_file);

                    auto nspace_parse_writers = nspace_parse->make_item < bgen::gen::group > ();
                    auto nspace_parse_readers = nspace_parse->make_item < bgen::gen::group > ();

                    for (auto & sp : types.structures) {
                        auto & s = sp.second;

                        nspace_parse_readers_predef->make_item < parser_reader_pre > (s);
                        nspace_parse_writers_predef->make_item < parser_writer_pre > (s);
                        nspace_parse_readers->make_item < parser_reader > (s);
                        nspace_parse_writers->make_item < parser_writer > (s);
                    }

                    // Generate http listeners
                    auto nlisteners = nspace->make_item<bgen::gen::cpp::cpp_namespace> (
                            vector <string > { "listeners" }
                    );

                    // read listener boilerplate
                    string listener_bplate_file = system::merge_path (system::get_executable_dir (), listener_boilerplate_location);

                    nlisteners->make_item<casa::gen::boilerplate>(listener_bplate_file);

                    for (auto & s : types.services) {
                        if (!s->param_type)
                            nlisteners->make_item<listener_get>(s);
                        else
                            nlisteners->make_item<listener_post>(s);

                    }

                    // Generate user entry point
                    nspace->make_item<register_listeners>(types.services);

                    // write output
                    rest_server_header.write ();
                }
            }
        }
    }
}