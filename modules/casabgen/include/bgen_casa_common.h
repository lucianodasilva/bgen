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

        js_type js_type_from_native (const type_info * type);

        struct id_t {
            string name;
            bgen::namespace_info nspace;

            inline bool operator < (const casa::id_t & idv) const {
                return
                        name < idv.name ||
                        nspace < idv.nspace;
            }
        };

        struct simple_struct;

        struct simple_type {
            js_type                         js;
            shared_ptr < simple_struct >    structure;
            shared_ptr < simple_type >      array_type;
            const type_info *               native_type;
        };

        struct simple_field {
            string                      name;
            shared_ptr < simple_type >  type;
        };

        struct simple_struct {
            casa::id_t              id;
            vector < simple_field > fields;
            const struct_info *     native_struct;
        };

        struct service {
            casa::id_t          id;
            shared_ptr < simple_type >  return_type;
            shared_ptr < simple_type >  param_type;
            method_info                 native_method;
        };

        struct type_map {
            map < string, shared_ptr < simple_type > >  types;
            vector < shared_ptr < service > >           services;
            map < id_t, shared_ptr < simple_struct > >  structures;
        };

    }
}

#endif