#include "bgen_casa_generation.h"
#include <bgen_core.h>

#include <fstream>
#include <sstream>

using namespace std;

namespace bgen {
    namespace casa {
        
        namespace gen {
            
            string namespace_to_string (const namespace_info & info, const string & separator) {
                stringstream stream;
                
                size_t size = info.size ();
                
                if (size > 0)
                    stream << info[0];
                
                for (int i = 1; i < size; ++i)
                    stream << separator << info [i];
                
                return stream.str ();
            }
            
            string namespace_to_uri (const namespace_info & info) {
                return namespace_to_string (info, "/");
            }
            
            string namespace_to_listener (const namespace_info & info) {
                return namespace_to_string (info, "_");
            }
            
            string method_listener_name (const method_info & info, const string & rest_method) {
                stringstream stream;
                
                stream
                    << "handle_"
                    << namespace_to_listener(info.namespace_name ())
                    << "_"
                    << info.name ()
                    << "_"
                    << rest_method;
                
                return stream.str ();
            }
            
            parser_reader_pre::parser_reader_pre (const struct_info::shared & info) : _info (info) {}
            
            void parser_reader_pre::write (bgen::gen::output & out) const {
                out.line ()
                    << "inline bool read (const value & source, "
                    << _info->native_name ()
                    << " & dest );";
            }
            
            parser_reader::parser_reader (const struct_info::shared & info) : _info (info) {}
            
            void parser_reader::write (bgen::gen::output & out) const {
                out.line ()
                    << "inline bool read (const value & source, "
                    << _info->native_name ()
                    << " & dest ) {";
                
                ++out.indent;
                
                out.line ()
                    << "if (!source.is_object()) return false;";
                out.line ()
                    << "auto source_obj = source.as_object ();";
                out.line ();
                out.line () << "bool success = true;";
                
                for ( auto & f : _info->fields () ) {
                    out.line () << "success = read_field_details (source_obj, \""
                        << f.name () << "\", dest." << f.name () <<
                        ") && success;";
                }
                
                out.line ()
                    << "return success;";
                
                --out.indent;
                
                out.line () << "}";
                out.line ();
            }
            
            parser_writer::parser_writer (const struct_info::shared & info) : _info (info) {}
            
            void parser_writer::write (bgen::gen::output & out) const {
                
                out.line ()
                    << "inline value write (const "
                    << _info->native_name ()
                    << " & source ) {";
                
                ++out.indent;
                
                out.line ()
                    << "auto object = web::json::value::object ();";
                
                for ( auto & f : _info->fields () ) {
                    out.line ()
                        << "object [\"" << f.name () << "\"] = write (source."
                        << f.name () << ");";
                }
                
                out.line ()
                    << "return object;";
                
                --out.indent;
                
                out.line ()
                    << "}";
            }
            
            parser_writer_pre::parser_writer_pre (const struct_info::shared & info) : _info (info) {}
            
            void parser_writer_pre::write (bgen::gen::output & out) const {
                out.line ()
                    << "inline value write (const "
                    << _info->native_name ()
                    << " & source );";
            }
            
            listener_post::listener_post (const method_info & info) :
                _info (info)
            {}
            
            void listener_post::write (bgen::gen::output & out) const {
                
                out.line ()
                    << "inline void "
                    << method_listener_name(_info, "post")
                    << " (http_request request) {";
                
                ++out.indent;
                
                out.line ()
                    << "handle_post <"
                    << _info.params ()[0].type ()->native_type_name () << ", "
                    << _info.return_type ()->native_type_name() << ", "
                    << namespace_to_string (_info.namespace_name (), "::") << "::" << _info.name ()
                    << "> (request);";
                
                --out.indent;
                
                out.line ()
                    << "}";
            }
            
            listener_get::listener_get (const method_info & info) :
            _info (info)
            {}
            
            void listener_get::write (bgen::gen::output & out) const {
                
                out.line ()
                    << "inline void "
                    << method_listener_name(_info, "get")
                    << " (http_request request) {";
                
                ++out.indent;
                
                out.line ()
                    << "handle_get <"
                    << _info.return_type ()->native_type_name() << ", "
                    << namespace_to_string (_info.namespace_name (), "::") << "::" << _info.name ()
                    << "> (request);";
                
                --out.indent;
                
                out.line ()
                    << "}";
            }
            
            register_listeners::register_listeners ( const struct_info::shared & info ) : _info (info) {}
            
            
            void register_listeners::write(bgen::gen::output & out) const {
                
                out.line ()
                    << "inline std::vector < http_listener > register_listeners ( const std::string & address ) {";
                
                ++out.indent;
                
                out.line ()
                    << "std::vector < http_listener > lv;";
                
                out.line ()
                    << "lv.reserve (" << _info->methods().size() << ");";
                
                out.line ()
                    << "uri_builder builder (address);";
                
                int i = 0;
                
                for (auto & m : _info->methods ()) {
                    out.line ()
                        << "builder.set_path(\"" << namespace_to_uri (m.namespace_name ()) << "/" << m.name () << "\");";
                    out.line ()
                        << "lv [" << i << "] = http_listener (builder.to_uri().to_string());";
                    
                    if (m.params ().size () == 0) {
                        out.line ()
                            << "lv [" << i << "].support(methods::GET, listeners::" << method_listener_name(m, "get") << ");";
                    } else {
                        out.line ()
                            << "lv [" << i << "].support(methods::POST, listeners::" << method_listener_name(m, "post") << ");";
                    }
                    
                    out.line ()
                        << "lv [" << i << "].open().wait();";
                    
                    ++i;
                }
                
                out.line ()
                    << "return lv;";
                
                --out.indent;
                
                out.line () << "}";
            }
            
            
            boilerplate::boilerplate ( const string & file )
            : _file_name (file) {}
            
            void boilerplate::write (bgen::gen::output & out) const {
                
                ifstream stream (_file_name);
                
                if (!stream) {
                    logger::write () << "failed to read resource file at " << _file_name;
                    error_status::fail();
                    return;
                }
                
                string line;
                
                while (getline (stream, line))
                    out.line () << line;
            }
            
        }
    
        const string parser_boilerplate_location = "resources/casablanca/parser_boilerplate.txt";
        
        const string listener_boilerplate_location = "resources/casablanca/listener_boilerplate.txt";

    }
}