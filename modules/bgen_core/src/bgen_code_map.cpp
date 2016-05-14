#include "bgen_code_map.h"

namespace bgen {
    
    template < class _t >
    inline _t * get_or_make (map < string, unique_ptr < _t > > & map_ref, const string & native_name) {
        
        auto element_it = map_ref.find (native_name);
        if (element_it != map_ref.end ())
            return element_it->second.get ();
        
        _t * new_element = new _t ();
        new_element->native_name = native_name;
        map_ref [native_name] = unique_ptr < _t > { new_element };
        
        return new_element;
    }
    
    struct_info * get_or_make_struct (code_map & symbols, const string & native_name) {
        return get_or_make (symbols.structs, native_name);
    }
    
    type_info * get_or_make_type (code_map & symbols, const string & native_name) {
        return get_or_make (symbols.types, native_name);
    }
    
}
