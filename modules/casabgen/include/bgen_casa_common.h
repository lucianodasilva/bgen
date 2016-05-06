#pragma once
#ifndef _bgen_casa_common_h_
#define _bgen_casa_common_h_

#include <cinttypes>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <bgen_core.h>

using namespace std;

namespace bgen {
    namespace casa {

        enum struct js_type : uint32_t {
            unknown,
            js_void,
            js_int,
            js_float,
            boolean,
            string,
            array,
            object
        };

        js_type js_type_from_native (const type_info::shared & type);

        struct id_t {
            string name;
            bgen::namespace_info nspace;

            inline bool operator < (const casa::id_t & idv) const {
                return
                        name < idv.name ||
                        nspace < idv.nspace;
            }
        };

        struct structure;

        struct type {
            js_type                  js;
            shared_ptr < structure > structure;
            shared_ptr < type >      array_type;
            type_info::shared        native_type;
        };

        struct field {
            string              name;
            shared_ptr < type > type;
        };

        struct structure {
            casa::id_t          id;
            vector < field >    fields;
            struct_info::shared native_struct;
        };

        struct service {
            casa::id_t          id;
            shared_ptr < type > return_type;
            shared_ptr < type > param_type;
            method_info         native_method;
        };

        struct type_map {
            map < string, shared_ptr < type > > types;
            vector < shared_ptr < service > > services;
            map < id_t, shared_ptr < structure > > structures;
        };

    }
}

#endif