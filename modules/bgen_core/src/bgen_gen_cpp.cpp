#include "bgen_gen_cpp.h"

#include <string>

using namespace std;

namespace bgen {
    namespace gen {

        namespace cpp {

            header_guard::header_guard(string guard_v) : guard (guard_v)
            {}

            void header_guard::write(output & out) const {
                out.line () << "#pragma once";
                out.line () << "#ifndef " << guard;
                out.line () << "#define " << guard;

                group::write (out);

                out.line () << "#endif";
            }

            // includes
            
            includes::includes ( const vector < string > & files ) : _files(files) {}

            void includes::write(output & out) const {
                for (auto & f : _files)
                    out.line () << "#include <" << f << ">";

                if (_files.size () > 0)
                    out.line();
            }

            // namespace

            cpp_namespace::cpp_namespace(namespace_info &info) : info (info)
            { }

            void cpp_namespace::write(output & out) const {

                size_t size = info.size ();

                for (auto & i : info) {
                    out.line () << "namespace " << i << "{";
                    ++out.indent;
                }

                if (size > 0)
                    out.line ();

                group::write (out);

                for (size_t i = 0; i < size; ++i) {
                    --out.indent;
                    out.line () << "}";
                }
            }

        }

    }
}