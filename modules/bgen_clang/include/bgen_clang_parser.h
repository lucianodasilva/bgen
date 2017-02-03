#pragma once
#ifndef _bgen_clang_parser_h_
#define _bgen_clang_parser_h_

#include <bgen_core.h>

namespace bgen {
    namespace clang {

        class parser : public bgen::source::parser {
        private:
        public:

            virtual source::parser_result parse ( bgen::parameters & params ) override;

        };

    }
}

#endif