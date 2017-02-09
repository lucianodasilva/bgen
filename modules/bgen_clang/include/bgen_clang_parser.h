#pragma once
#ifndef _bgen_clang_parser_h_
#define _bgen_clang_parser_h_

#include <bgen_core.h>
#include <string>

using namespace std;

namespace bgen {
    namespace clang {

        class parser : public bgen::source::parser {
        public:

            virtual source::parser_result parse ( context & cxt ) override;

        };

    }
}

#endif