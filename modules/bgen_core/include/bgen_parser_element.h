#pragma once
#ifndef _bgen_parser_element_h_
#define _bgen_parser_element_h_

#include <memory>
#include <string>
#include <vector>

using namespace std;

namespace bgen{
    namespace parser {

        struct native_handle;

        struct element_flags {
            bool is_virtual : 1;
            bool is_pure : 1;
            bool is_static : 1;
            bool is_const : 1;
            bool is_ctor : 1;
        };

        enum class element_kind : uint32_t {
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
            element_kind            kind;
            element_flags           flags {};
            unique_ptr < native_handle > native;
        };

        using element_vector = vector < element >;

        using semantic_path = vector < string >;

    }
}

#endif