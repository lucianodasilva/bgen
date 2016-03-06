#include "bgen_gen_file.h"

#include <fstream>
#include "bgen_logger.h"

using namespace std;

namespace bgen {
    namespace gen {

        file::file(const string &filename) : _filename (filename)
        { }

        void file::write () const {

            ofstream stream (_filename.c_str ());

            if (!stream) {
                logger::write () << "failed to open output file: " << _filename;
                return;
            }

            output out {stream};
            
            group::write (out);
        }

        void file::write(output & out) const {
            write ();
        }

    }
}