#pragma once
#ifndef _bgen_system_h_
#define _bgen_system_h_

#include <string>

using namespace std;

namespace bgen {
    namespace system {
        
        string get_executable_path ();
        string get_executable_dir ();
        
        char get_path_separator ();
        
        string merge_path ( const string & p1, const string & p2);
        
    }
}

#endif