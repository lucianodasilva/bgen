#include "bgen_system.h"
#include "bgen_details.h"

#include <cinttypes>

#if defined (BGEN_API_WIN32)

#define WIN32_LEAN_AND_MEAN 1
#include <stdio.h>
#include <Windows.h>

namespace bgen {
    namespace system {
        
    }
}

#elif defined (BGEN_API_UNIX)

#include <cstdio>
#include <iostream>
#include <memory>

namespace bgen {
    namespace system {
        
        string compilers [] = {
            "clang++",
            "g++"
        };
        
        string execute (const string & command) {
            char buffer [256];
            string result;
            
            shared_ptr < FILE > pipe(popen (command.c_str (), "r"), pclose);
            
            if (!pipe)
                return "";
            
            while (!feof(pipe.get ())) {
                if (fgets (buffer, 256, pipe.get ()) != nullptr)
                    result += buffer;
            }
            
            return result;
        }
        
        vector < string > get_include_paths () {
            // find available compiler
            string not_found = "not found";
            string compiler_path = "";
            
            for (auto & compiler_name : compilers) {
                auto exec_ret = execute ("which " + compiler_name);
                
                if (exec_ret.size () < not_found.size ())
                    continue;
                
                if (!std::equal (
                    exec_ret.rbegin (),
                    exec_ret.rend () + not_found.size (),
                    not_found.rbegin ()
                )) {
                    compiler_path = exec_ret;
                    continue;
                }
            }
            
            if (compiler_path.empty ())
                return {};
            
            // check installation info
            string compiler_info = execute (compiler_path + "-v -x c++ -E - 2>&1");
            if (compiler_info.empty ())
                return {};
            
            
        }
    }
}

#endif
