#pragma once

#ifndef _bgen_source_parser_h_
#define _bgen_source_parser_h_

#include "bgen_parameters.h"
#include "bgen_context.h"
#include "bgen_source_element.h"
#include "bgen_source_info.h"

namespace bgen {
    namespace source {

        struct parser_result {
            element_vector elements;
            struct_vector structs;
            type_vector types;
        };

        class parser {
        public:
            virtual parser_result parse ( context & cxt, const parameters & params ) = 0;
        };

    }
}

#endif