#pragma once
#ifndef _bgen_source_element_h_
#define _bgen_source_element_h_

#include "bgen_source_info.h"

namespace bgen {
    namespace source {

        struct element_flags {
            bool is_virtual : 1;
            bool is_pure : 1;
            bool is_static : 1;
            bool is_const : 1;
            bool is_ctor : 1;
        };

        enum class element_type : uint32_t {
            undefined,
            decl_class,
            decl_struct,
            decl_base,
            decl_namespace,
            method,
            function,
            parameter,
            field,
        };

        struct element {
            source::location        location;
            string                  name;
            source::element_type    element_type { element_type::undefined };
            element_flags           flags {};
            type_id_t               type_id { type_id_t::empty };
        };

        using element_vector = vector < element >;

    }
}

#endif