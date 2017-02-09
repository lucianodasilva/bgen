#pragma once
#ifndef _bgen_source_dependency_h_
#define _bgen_source_dependency_h_

#include <string>
#include <memory>
#include <vector>

using namespace std;

namespace bgen {
    namespace source {
    
        class dependency {
        private:
            size_t  _struct_id;
            bool    _is_soft { false };
        public: 
        
            dependency () = default;
            inline dependency (size_t struct_id, bool soft_dep) :
                _struct_id (struct_id),
                _is_soft (soft_dep)
            {}
            
            inline size_t struct_id () const { return _struct_id; }
            inline bool is_soft () const { return _is_soft; }

        };

        using dependency_vector = vector < size_t >;

    }
}

#endif