#pragma once
#ifndef _bgen_source_map_h_
#define _bgen_source_map_h_

#include "bgen_details.h"
#include "bgen_source_struct.h"
#include "bgen_source_type.h"

namespace bgen {
    namespace source {

        struct map {
            struct_vector   structs;
            type_vector     types;
        };

        template < class _t, class _key_t >
        inline size_t get_or_make_id (indexed_vector < _key_t, _t > & vec, const _key_t & identifier) {
            size_t id = vec.find_index (identifier);

            if (id == indexed_vector < _key_t, _t >::nindex) {
                auto emplace_ret = vec.emplace_back (identifier);
                emplace_ret.second.identifier = identifier;
                id = emplace_ret.first;
            }

            return id;
        }

        inline struct_id get_or_make_struct_id (map & source_map, const string & identifier) {
            return get_or_make_id (source_map.structs, identifier);
        }

        inline type_id get_or_make_type_id (map & source_map, const string & identifier) {
            return get_or_make_id (source_map.types, identifier);
        }

    }
}

#endif