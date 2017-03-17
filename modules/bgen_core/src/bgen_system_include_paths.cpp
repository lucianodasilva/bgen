#include "bgen_system.h"
#include "bgen_common_details.h"

#include <cinttypes>
#include <regex>

#if defined (BGEN_API_WIN32)

#define WIN32_LEAN_AND_MEAN 1
#include <stdio.h>
#include <Windows.h>

namespace bgen {
    namespace system {
        
        // nop implementation for windows
        vector < string > get_include_paths () { return {}; }

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
            
            // remove trailing line feed
            if (*result.rbegin () == '\n')
                result.resize(result.size() - 1);
            
            return result;
        }
        
        vector < string > get_include_paths () {
            string not_found = "not found";
            string reg_expression = "\\s(\\/((?!framework).)*)+\n";
            
            // find available compiler
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
                    break;
                }
            }
            
            if (compiler_path.empty ())
                return {};
            
            // check installation info
            string compiler_info = execute ("echo "" | " + compiler_path + " -v -x c++ -E - 2>&1");
            if (compiler_info.empty ())
                return {};
            
            regex reg_exp (reg_expression, regex_constants::ECMAScript);
            smatch matches;
            
            vector < string > includes;
            
            while (regex_search (compiler_info, matches, reg_exp)) {
                if (matches.size () > 0)
                    includes.push_back (matches [1]);
                
                compiler_info = matches.suffix().str ();
            }
    
            return includes;
        }
    }
}

#endif
