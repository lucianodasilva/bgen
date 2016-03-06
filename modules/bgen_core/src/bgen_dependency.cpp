#include "bgen_dependency.h"
#include "bgen_struct_info.h"
#include "bgen_logger.h"

#include <map>
#include <string>
#include <vector>

using namespace std;

namespace bgen {
    
    dependency::dependency () 
        : _struct_name (""), _is_soft (false) 
    {}
    
    dependency::dependency ( const string & name, bool soft_dep )
        : _struct_name (name), _is_soft (soft_dep) 
    {}
    
    enum struct dep_item_state : uint32_t {
        none,
        in_work,
        done
    };
        
    struct dep_item {
        struct_info::shared strct;
        dep_item_state      state;
    };
    
    using dep_map_t = map < string, dep_item >;
        
    void run_item_depth (  dep_item & item, dep_map_t & map, vector < shared_ptr < struct_info > > & out ) {
        
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
    
    void dependency::sort ( vector < shared_ptr < struct_info > > & dep_structs ) {
        
        vector < shared_ptr < struct_info > > sorted;
        
        // fill dependency item map
        map < string, dep_item > in_work;
        
        for ( auto & s : dep_structs ) {
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
        
        swap (sorted, dep_structs);
        
    }
    
}