#include "bgen_struct_info.h"
#include "bgen_logger.h"

#include <map>
#include <vector>

namespace bgen {
    
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
            
            auto const & deps = item.strct->dependencies;
            
            for ( auto & d : deps ) {
                auto const & dep_name = d.struct_name ();
                
                auto it = map.find (dep_name);
                if (it != map.end ()){
                    
                    auto & dep = it->second;
                    
                    if (dep.state == dep_item_state::in_work) {
                        logger::write () << "cyclic dependency detected in " << item.strct->native_name;
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
            in_work [s->native_name] = {
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