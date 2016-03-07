#include "bgen_system.h"
#include "bgen_details.h"

#include <cinttypes>

#if defined (BGEN_OS_WINDOWS)

#elif defined (BGEN_OS_LINUX)

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <limits>
#include <cinttypes>

namespace bgen {
    namespace system {
        
        string get_executable_path () {
            char * path = realpath ("/prod/self/exe", nullptr);
            
            if (!path)
                return "";
            
            string result = path;
            free (path);
            return result;
        }
        
    }
}


#elif defined (BGEN_OS_DARWIN)

#include <mach-o/dyld.h>
#include <limits.h>
#include <stdlib.h>
#include <string>
#include <dlfcn.h>

namespace bgen {
    namespace system {
        
        string get_executable_path () {
            
            char * buffer = new char [PATH_MAX];
            
            // safelly clean up memory
            auto_guard ([&buffer]() {
                delete [] buffer;
            });

            string resolved;
            
            uint32_t size = PATH_MAX;
                
            if (_NSGetExecutablePath(buffer, &size) == -1) {
                delete [] buffer;
                buffer = new char [size];
                
                if (!_NSGetExecutablePath(buffer, &size))
                    return "";
            }
                
            char * real_path_buffer = realpath(buffer, nullptr);
            
            if (!real_path_buffer)
                return "";
                
            resolved = real_path_buffer;
                
            free (real_path_buffer);
   
            return resolved;
        }
        
    }
}

#endif

#if defined (BGEN_API_WIN32)

namespace bgen {
    namespace system {
        
        char get_path_separator () {
            return '\\';
        }
        
    }
}

#elif defined (BGEN_API_UNIX)

namespace bgen {
    namespace system {

        char get_path_separator () {
            return '/';
        }
        
    }
}

#endif

namespace bgen {
    namespace system {
        
        string get_executable_dir () {
            string path = get_executable_path();
            
            auto pos = path.find_last_of (get_path_separator());
            
            if (pos == string::npos)
                return path;
            
            return path.substr(0, pos + 1);
        }
        
        string merge_path ( const string & p1, const string & p2) {
            
            string::const_iterator
                p1_start = p1.begin (),
                p1_end = p1.end (),
                p2_start = p2.begin (),
                p2_end = p2.end ();
            
            char separator = get_path_separator();
            
            if (p1_start != p1_end && *(p1_end - 1) == separator)
                --p1_end;
            
            if (p2_start != p2_end && *(p2_start) == separator)
                ++p2_start;
            
            string res;
            
            res.reserve (p1.size () + p2.size () + 1);
            res.append (p1_start, p1_end);
            res.append (&separator, &separator + 1);
            res.append (p2_start, p2_end);
            
            return res;
        }
        
    }
}
