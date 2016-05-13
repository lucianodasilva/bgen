#pragma once
#ifndef _bgen_code_map_h_
#define _bgen_code_map_h_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "bgen_struct_info.h"
#include "bgen_type_info.h"

using namespace std;

namespace bgen {
    
    class visitor;
    
    using dependency_vector = vector < struct_info * >;
    using struct_map = map < string, unique_ptr < struct_info > >;
    using type_map = map < string, unique_ptr < type_info > >;

    class code_map {
    private:

        unique_ptr < struct_info >  _global;
        
		type_map                    _types;
		struct_map                  _structs;
        dependency_vector           _sorted_dependencies;

    public:

		code_map ();

		const struct_info &         global () const;
        
        const type_map &            types () const;
        const struct_map &          structs () const;
        
        const dependency_vector &   sorted_dependencies () const;

        friend class visitor;
    };

}

#endif
