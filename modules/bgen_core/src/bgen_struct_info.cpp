#include "bgen_struct_info.h"
#include "bgen_logger.h"

#include <map>
#include <vector>

namespace bgen {
    
    struct_info::struct_info (
        const string & name_v,
        const string & native_name_v,
        const namespace_info & namespace_name_v,
        const struct_vector & base_structs_v,
        const field_vector & fields_v,
        const method_vector & methods_v,
        const vector < dependency > & dependencies_v,
        const source_location & location_v,
        bool is_visited_v,
        struct_type type_v
    ) :
    	_name (name_v),
    	_native_name (native_name_v),
        _namespace_name (namespace_name_v),
    	_base_structs (base_structs_v),
        _fields (fields_v),
        _methods (methods_v),
        _dependencies (dependencies_v),
        _location (location_v),
        _is_visited (is_visited_v),
        _type (type_v)
    {}
    
	const string & struct_info::name() const {
		return _name;
	}

    const string & struct_info::native_name () const {
        return _native_name;
    }
    
    const namespace_info & struct_info::namespace_name() const {
        return _namespace_name;
    }
    
    const struct_vector & struct_info::base_structs () const {
        return _base_structs;
    }
    
    const vector < field_info > & struct_info::fields () const {
        return _fields;
    }
    
    const vector < method_info > & struct_info::methods () const {
        return _methods;
    }
    
    const vector < dependency > & struct_info::dependencies () const {
        return _dependencies;
    }
    
    const source_location & struct_info::location () const {
        return _location;
    }
    
    bool struct_info::is_visited() const {
        return _is_visited;
    }
    
    struct_type struct_info::type() const {
        return _type;
    }
    
    namespace dependency_arrange_details {
        
        enum struct dep_item_state : uint32_t {
            none,
            in_work,
            done
        };
        
        struct dep_item {
            struct_info * strct;
            dep_item_state      state;
        };
        
        using dep_map = map < string, dep_item >;
        
        void run_item_depth (  dep_item & item, dep_map & map, struct_vector & out ) {
            
            item.state = dep_item_state::in_work;
            
            auto const & deps = item.strct->dependencies ();
            
            for ( auto & d : deps ) {
                auto const & dep_name = d.struct_name ();
                
                auto it = map.find (dep_name);
                if (it != map.end ()){
                    
                    auto & dep = it->second;
                    
                    if (dep.state == dep_item_state::in_work) {
                        logger::write () << "cyclic dependency detected in " << item.strct->native_name();
                        // TODO: handle cyclic dependency
                        // TODO: remember soft / hard dependencies
                        // TODO: temporary behaviour
                        continue;
                    }
                    
                    if (dep.state == dep_item_state::done)
                        continue;
                    
                    run_item_depth (dep, map, out);
                }
            }
            
            out.push_back (item.strct);
            item.state = dep_item_state::done;
        }
        
    }
    
    struct_vector arrange_by_dependencies (const struct_vector & structs) {
        using namespace dependency_arrange_details;
        struct_vector sorted;
        
        // fill dependency item map
        dep_map in_work;
        
        for ( auto & s : structs ) {
            in_work [s->native_name()] = {
                s,
                dep_item_state::none
            };
        }
        
        // run sort
        for ( auto & dep_it : in_work ) {
            auto state = dep_it.second.state;
            
            if (state == dep_item_state::none)
                run_item_depth (dep_it.second, in_work, sorted);
        }
        
        return sorted;
    }


}