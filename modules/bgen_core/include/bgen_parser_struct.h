#pragma once
#ifndef _bgen_parser_struct_h_
#define _bgen_parser_struct_h_

#include "bgen_common.h"
#include "bgen_parser_cursor.h"

#include "bgen_common_indexed_vector.h"

#include <vector>

namespace bgen {
    namespace parser {


        using semantic_name = vector < string >;

        semantic_name semantic_name_from_path (const parser::semantic_path & path);

        using type_id = size_t;

        struct field_info {
            semantic_name       path;
            parser::location    location; 
            string              identifier;
            type_id             type;
            parser::visibility  visibility;
        };

        using field_vector = vector < field_info >;

        struct param_info {
            string  identifier;
            type_id type;
        };

        using param_vector = vector < param_info >;

        struct method_info {
            semantic_name           path;
            parser::location        location;
            string                  identifier;
            
            param_vector            parameters;
            type_id                 return_type;
            parser::visibility      visibility;
            parser::cursor_flags    flags;
        };

        using method_vector = vector < method_info >;

        using struct_id = size_t;

        using struct_id_vector = vector < struct_id >;

        enum struct struct_kind : uint32_t {
            struct_unknown,
            struct_struct,
            struct_class
        };

        struct struct_info {
            pool_span < field_info >    fields;
            pool_span < method_info >   methods;

            semantic_name           name;
            parser::location        location;
            string                  identifier;
            string                  native_identifier;

            struct_id_vector        parent_struct_ids;
            struct_id_vector        dependencies;

            struct_kind             kind;
        };

        using struct_vector = bgen::indexed_vector < string, struct_info >;

        using struct_id = size_t;
    }
}

#endif