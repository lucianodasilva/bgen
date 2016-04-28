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

                string method_listener_name(const method_info &info, const string &rest_method) {
                    stringstream stream;

                    stream
                    << "handle_"
                    << gen::namespace_to_listener(info.namespace_name())
                    << "_"
                    << info.name()
                    << "_"
                    << rest_method;

                    return stream.str();
                }

                parser_reader_pre::parser_reader_pre(const struct_info::shared &info) : _info(info) { }

                void parser_reader_pre::write(bgen::gen::output &out) const {
                    out.line()
                    << "inline bool read (const value & source, "
                    << _info->native_name()
                    << " & dest );";
                }

                parser_reader::parser_reader(const struct_info::shared &info) : _info(info) { }

                void parser_reader::write(bgen::gen::output &out) const {
                    out.line()
                    << "inline bool read (const value & source, "
                    << _info->native_name()
                    << " & dest ) {";

                    ++out.indent;

                    out.line()
                    << "if (!source.is_object()) return false;";
                    out.line()
                    << "auto source_obj = source.as_object ();";
                    out.line();
                    out.line() << "bool success = true;";

                    for (auto &f : _info->fields()) {
                        out.line() << "success = read_field_details (source_obj, \""
                        << f.name() << "\", dest." << f.name() <<
                        ") && success;";
                    }

                    out.line()
                    << "return success;";

                    --out.indent;

                    out.line() << "}";
                    out.line();
                }

                parser_writer::parser_writer(const struct_info::shared &info) : _info(info) { }

                void parser_writer::write(bgen::gen::output &out) const {

                    out.line()
                    << "inline value write (const "
                    << _info->native_name()
                    << " & source ) {";

                    ++out.indent;

                    out.line()
                    << "auto object = web::json::value::object ();";

                    for (auto &f : _info->fields()) {
                        out.line()
                        << "object [\"" << f.name() << "\"] = write (source."
                        << f.name() << ");";
                    }

                    out.line()
                    << "return object;";

                    --out.indent;

                    out.line()
                    << "}";
                }

                parser_writer_pre::parser_writer_pre(const struct_info::shared &info) : _info(info) { }

                void parser_writer_pre::write(bgen::gen::output &out) const {
                    out.line()
                    << "inline value write (const "
                    << _info->native_name()
                    << " & source );";
                }

                listener_post::listener_post(const method_info &info) :
                        _info(info) { }

                void listener_post::write(bgen::gen::output &out) const {

                    out.line()
                    << "inline void "
                    << method_listener_name(_info, "post")
                    << " (http_request request) {";

                    ++out.indent;

                    out.line()
                    << "handle_post <"
                    << _info.params()[0].type()->native_type_name() << ", "
                    << _info.return_type()->native_type_name() << ", "
                    << namespace_to_string(_info.namespace_name(), "::") << "::" << _info.name()
                    << "> (request);";

                    --out.indent;

                    out.line()
                    << "}";
                }

                listener_get::listener_get(const method_info &info) :
                        _info(info) { }

                void listener_get::write(bgen::gen::output &out) const {

                    out.line()
                    << "inline void "
                    << method_listener_name(_info, "get")
                    << " (http_request request) {";

                    ++out.indent;

                    out.line()
                    << "handle_get <"
                    << _info.return_type()->native_type_name() << ", "
                    << namespace_to_string(_info.namespace_name(), "::") << "::" << _info.name()
                    << "> (request);";

                    --out.indent;

                    out.line()
                    << "}";
                }

                register_listeners::register_listeners(const struct_info::shared &info) : _info(info) { }


                void register_listeners::write(bgen::gen::output &out) const {

                    out.line()
                    << "inline std::vector < http_listener > register_listeners ( const std::string & address ) {";

                    ++out.indent;

                    out.line()
                    << "std::vector < http_listener > lv;";

                    out.line()
                    << "lv.reserve (" << _info->methods().size() << ");";

                    out.line()
                    << "uri_builder builder (address);";

                    int i = 0;

                    for (auto &m : _info->methods()) {
                        out.line()
                        << "builder.set_path(\"" << namespace_to_uri(m.namespace_name()) << "/" << m.name() << "\");";
                        out.line()
                        << "new (&lv [" << i << "]) http_listener (builder.to_uri().to_string());";

                        if (m.params().size() == 0) {
                            out.line()
                            << "lv [" << i << "].support(methods::GET, listeners::" << method_listener_name(m, "get") <<
                            ");";
                        } else {
                            out.line()
                            << "lv [" << i << "].support(methods::POST, listeners::" <<
                            method_listener_name(m, "post") <<
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
                        type_map & types,
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

                    auto source_types = types.sorted_structs();

                    // Generate predefinition
                    for (auto & s : source_types) {
                        if (s->is_visited())
                            nspace_parse->make_item < parser_reader_pre > (s);
                    }

                    for (auto & s : source_types) {
                        if (s->is_visited())
                            nspace_parse->make_item < casa::gen::cpp::parser_writer_pre > (s);
                    }

                    // Write boilerplate
                    string parser_bplate_file = system::merge_path(system::get_executable_dir(), parser_boilerplate_location);

                    nspace_parse->make_item<casa::gen::boilerplate>(parser_bplate_file);

                    // Generate serialization reader
                    for (auto & s : source_types) {
                        if (s->is_visited())
                            nspace_parse->make_item < parser_reader > (s);
                    }

                    // Generate serialization writer
                    for (auto & s : source_types) {
                        if (s->is_visited())
                            nspace_parse->make_item < parser_writer > (s);
                    }


                    // Generate http listeners
                    auto nlisteners = nspace->make_item<bgen::gen::cpp::cpp_namespace> (
                            vector <string > { "listeners" }
                    );

                    // read listener boilerplate
                    string listener_bplate_file = system::merge_path (system::get_executable_dir (), listener_boilerplate_location);

                    nlisteners->make_item<casa::gen::boilerplate>(listener_bplate_file);

                    for (auto & m : types.global()->methods ()) {
                        if (m.params ().size () == 0)
                            nlisteners->make_item<listener_get>(m);
                        else if (m.params ().size () == 1)
                            nlisteners->make_item<listener_post>(m);
                        else
                            logger::write(m.location ()) << "unsupported signature for rest service";
                    }

                    // Generate user entry point
                    nspace->make_item<register_listeners>(types.global());

                    // write output
                    rest_server_header.write ();
                }
            }
        }
    }
}