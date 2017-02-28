#pragma once
#ifndef _bgen_parser_driver_h_
#define _bgen_parser_driver_h_

#include "bgen_parser_cursor.h"
#include "bgen_parser_type.h"

namespace bgen {
    namespace parser {

        class driver {
        public:

            // types
            virtual type get_canonical_type (const parser::type & type) const = 0;
            virtual type get_pointer_type (const parser::type & type) const = 0;
            virtual cursor get_type_declaration (const parser::type & type) const = 0;
            virtual bool is_const_qualified (const parser::type & type) const = 0;
            virtual type get_array_element_type (const parser::type & type) const = 0;

            virtual template_arg_vector get_template_arguments (const parser::type & type) const = 0;

            // cursors
            virtual visibility get_visibility (const parser::cursor & cursor) const = 0;
            virtual type get_type (const parser::cursor & cursor) const = 0;
            virtual semantic_path get_semantic_path (const parser::cursor & cursor) const = 0;
        };

        class visitor_controller {
        public:
            virtual void visit_children () const = 0;
        };
        
    }
}

#endif