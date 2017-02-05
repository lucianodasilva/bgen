#pragma once
#ifndef _bgen_dependency_h_
#define _bgen_dependency_h_

#include <string>
#include <memory>
#include <vector>

using namespace std;

namespace bgen {
    
    class dependency {
    private:
        string  _struct_name;
        bool    _is_soft { false };
    public: 
    
        dependency () = default;
        inline dependency (const string & name, bool soft_dep) :
            _struct_name (name),
            _is_soft (soft_dep)
        {}
        
        inline const string & struct_name () const { return _struct_name; }
        inline bool is_soft () const { return _is_soft; }

    };
    
}

#endif