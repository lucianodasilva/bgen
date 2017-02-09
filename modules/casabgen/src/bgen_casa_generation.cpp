#include "bgen_casa_generation.h"

#include <fstream>

using namespace std;

namespace bgen {
    namespace casa {
        
        namespace gen {
            
            string id_to_string (const casa::id_t & id, const string & separator) {
                stringstream stream;
                
                size_t size = id.nspace.size ();
                
                if (size > 0)
                    stream << id.nspace[0];
                
                for (size_t i = 1; i < size; ++i)
                    stream << separator << id.nspace [i];

                if (size > 0)
                    stream << separator;

                stream << id.name;
                
                return stream.str ();
            }
            
            string id_to_uri (const casa::id_t & id) {
                return id_to_string (id, "/");
            }
            
            string id_to_listener (const casa::id_t & id) {
                return id_to_string (id, "_");
            }
            
            boilerplate::boilerplate ( const string & file )
            : _file_name (file) {}
            
            void boilerplate::write (context & cxt, bgen::gen::output & out) const {
                
                ifstream stream (_file_name);
                
                if (!stream) {
                    cxt.status.fail() << "failed to read resource file at " << _file_name;
                    return;
                }
                
                string line;
                
                while (getline (stream, line))
                    out.line () << line;
            }
        }
    }
}