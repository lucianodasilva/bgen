#include "bgen_source_struct_info.h"

#include <cinttypes>
#include <map>
#include <vector>

namespace bgen {

    namespace source {
        
        namespace details {
            
            enum struct dep_item_state : uint32_t {
                none,
                in_work,
                done
            };
            
            struct dep_item {
                size_t          struct_id;
                dep_item_state  state;
            };
            
            using dep_map = map < string, dep_item >;
            
            void run_item_depth ( const struct_vector & structs, dep_item & item, dep_map & map, struct_vector & out ) {
                
                item.state = dep_item_state::in_work;
                auto const & item_struct = structs [item.struct_id];
                
                for ( auto & d : item_struct.dependencies ) {
                    auto & dep_struct = structs[d.struct_id()];
                    auto it = map.find (dep_struct.name);
                    if (it != map.end ()){
                        
                        auto & dep = it->second;
                        
                        if (dep.state == dep_item_state::in_work) {
                            // logger::write () << "cyclic dependency detected in " << item.strct->native_name;
                            // TODO: handle cyclic dependency
                            // TODO: remember soft / hard dependencies
                            // TODO: temporary behaviour
                            continue;
                        }
                        
                        if (dep.state == dep_item_state::done)
                            continue;
                        
                        run_item_depth (structs, dep, map, out);
                    }
                }
                
                out.push_back (item_struct.name, item_struct);
                item.state = dep_item_state::done;
            }
        }
        
        struct_vector arrange_by_dependencies (const struct_vector & structs) {
            using namespace details;
            struct_vector sorted;
            
            // fill dependency item map
            dep_map in_work;
            
            for ( auto & s : structs) {
                in_work [s.native_name] = {
                    structs.find_index (s.native_name),
                    dep_item_state::none
                };
            }
            
            // run sort
            for ( auto & dep_it : in_work ) {
                auto state = dep_it.second.state;
                
                if (state == dep_item_state::none)
                    run_item_depth (structs, dep_it.second, in_work, sorted);
            }
            
            // fix dependencies ids

            return sorted;
        }
    }
}