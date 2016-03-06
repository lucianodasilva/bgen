#pragma once
#ifndef _bgen_type_map_h_
#define _bgen_type_map_h_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "bgen_struct_info.h"
#include "bgen_type_info.h"

using namespace std;

namespace bgen {
    
    class visitor;

    class type_map {
    private:

	struct_info::shared						_global;
        
		map < string, type_info::shared >	_types;
		map < string, struct_info::shared > _structs;
        vector < struct_info::shared >      _sorted_dependencies;

    public:

		type_map ();

		struct_info::shared global () const;

		type_info::shared make_type (const string & native_name);
		struct_info::shared make_struct (const string & native_name);

		bool has_type (const string & native_name) const;
		bool has_struct (const string & native_name) const;
        
		type_info::shared get_type(const string & native_name);
		struct_info::shared get_struct(const string & native_name);
        
        const vector < struct_info::shared > &sorted_structs() const;

        friend class visitor;
    };

}

#endif
