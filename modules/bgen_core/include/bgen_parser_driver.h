#pragma once
#ifndef _bgen_parser_driver_h_
#define _bgen_parser_driver_h_

#include "bgen_parser_element.h"
#include "bgen_parser_type.h"

namespace bgen {
    namespace parser {

        struct native_handle {};

        class driver {
        public:

            virtual const element_vector & elements () const = 0; 

            virtual type get_type (const element & e) const = 0;
            virtual type get_type (const type & t) const = 0;

            virtual template_parameter_vector get_template_parameters (const element & e) const = 0;
            virtual template_parameter_vector get_template_parameters (const type & t) const = 0;

            virtual semantic_path get_semantic_path (const element & e) const = 0;
            virtual semantic_path get_semantic_path (const type & t) const = 0;

        };

    }
}

#endif