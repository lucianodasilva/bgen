#include "bgen_gen_file.h"

#include <fstream>

using namespace std;

namespace bgen {
    namespace gen {

        file::file(const string &filename) : _filename (filename)
        { }

        void file::write (context & cxt) const {

            ofstream stream (_filename.c_str ());

            if (!stream) {
                cxt.status.fail() << "failed to open output file: " << _filename;
                return;
            }

            output out {stream};
            
            group::write (cxt, out);
        }

        void file::write(context & cxt, output & out) const {
            write (cxt);
        }

    }
}