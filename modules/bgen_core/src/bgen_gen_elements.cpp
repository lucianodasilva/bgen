#include "bgen_gen_elements.h"

#include <string>

using namespace std;

namespace bgen {
    namespace gen {

        // text

        text::text ( const string & content_v ) : content (content_v) {}

        void text::write(output & out) const {
            out.line () << content;
        }

        // group

        void group::write (output & out) const {
            for ( auto & i : _content)
                i->write (out);
        }

    }
}