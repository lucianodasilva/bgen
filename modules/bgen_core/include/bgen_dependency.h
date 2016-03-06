#pragma once
#ifndef _bgen_dependency_h_
#define _bgen_dependency_h_

#include <string>
#include <memory>
#include <vector>

using namespace std;

namespace bgen {
    
    struct struct_info;
    
    struct dependency {
    private:
        string  _struct_name;
        bool    _is_soft;
    public: 
    
        dependency ();
        dependency (const string & name, bool soft_deo);
        
        inline string struct_name () const { return _struct_name; } 
        inline bool is_soft () const { return _is_soft; }
        
        static void sort ( vector < shared_ptr < struct_info > > & dep_structs );

    };    
    
}

#endif