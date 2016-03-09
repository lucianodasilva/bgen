#include "bgen_casa_module.h"
#include "bgen_casa_generation.h"

#include <algorithm>

using namespace std;

namespace bgen {

    base_language_plugin * make_casabgen_plugin() {
        return new casablanca_plugin();
    }

    string casablanca_plugin::handled_language() const {
        return "casablanca";
    }

    bool casablanca_plugin::handles_language(const std::string & lang) const{
        return lang == "casablanca";
    }

    void casablanca_plugin::generate (type_map & types) const {
        
        auto & params = parameters::get ();
        
        // output file
        string rest_server_header_file = system::merge_path (params.host_dest, "/rest_server.h");
        
        gen::file rest_server_header = {rest_server_header_file};
        
        // output root element
        auto root = rest_server_header.make_item< gen::cpp::header_guard >("_rest_server_h_");
        
        root->make_item < gen::cpp::includes > ( vector <string> {
            "cpprest/json.h",
            "cpprest/http_listener.h",
            "cinttypes",
            "map",
            "memory",
            "string",
            "vector",
            "type_traits"
        });
        
        root->make_item < gen::text > ("using namespace web;");
        root->make_item < gen::text > ("using namespace web::json;");
        root->make_item < gen::text > ("using namespace web::http;");
        root->make_item < gen::text > ("using namespace web::http::experimental::listener;");
        
        root->make_item < gen::text > ("using namespace std;");
        
        auto nspace = root->make_item < gen::cpp::cpp_namespace > (
            vector < string > {"poww", "rest"}
        );
        
        auto nspace_parse = nspace->make_item < gen::cpp::cpp_namespace > (
            vector < string > { "parse" }
        );
        
        auto source_types = types.sorted_structs();
        
        // Generate predefinition
        for (auto & s : source_types) {
            if (s->is_visited())
                nspace_parse->make_item < casa::gen::parser_reader_pre > (s);
        }
        
        for (auto & s : source_types) {
            if (s->is_visited())
                nspace_parse->make_item < casa::gen::parser_writer_pre > (s);
        }
        
        // Write boilerplate
        string parser_bplate_file = system::merge_path(system::get_executable_dir(), casa::parser_boilerplate_location);
        
        nspace_parse->make_item<casa::gen::boilerplate>(parser_bplate_file);

        // Generate serialization reader
        for (auto & s : source_types) {
            if (s->is_visited())
                nspace_parse->make_item < casa::gen::parser_reader > (s);
        }
        
        // Generate serialization writer
        for (auto & s : source_types) {
            if (s->is_visited())
                nspace_parse->make_item < casa::gen::parser_writer > (s);
        }


        // Generate http listeners
        auto nlisteners = nspace->make_item<bgen::gen::cpp::cpp_namespace> (
            vector <string > { "listeners" }
        );
        
        // read listener boilerplate
        string listener_bplate_file = system::merge_path (system::get_executable_dir (), casa::listener_boilerplate_location);
        
        nlisteners->make_item<casa::gen::boilerplate>(listener_bplate_file);
        
        for (auto & m : types.global()->methods ()) {
            if (m.params ().size () == 0)
                nlisteners->make_item<casa::gen::listener_get>(m);
            else if (m.params ().size () == 1)
                nlisteners->make_item<casa::gen::listener_post>(m);
            else
                logger::write(m.location ()) << "unsupported signature for rest service";
        }

        // Generate user entry point
        nspace->make_item<casa::gen::register_listeners>(types.global());
        
        // write output
        rest_server_header.write ();

        // Generate javascript api???

    }


}