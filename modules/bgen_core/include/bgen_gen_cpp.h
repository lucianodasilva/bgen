#pragma once
#ifndef _bgen_gen_cpp_h_
#define _bgen_gen_cpp_h_

#include "bgen_gen_elements.h"
#include "bgen_struct_info.h"

using namespace std;

namespace bgen {
    namespace gen {

        namespace cpp {

            class header_guard : public group {
            public:

                using shared = shared_ptr < header_guard >;

                string guard;

                header_guard (string guard_v);

                virtual void write (output & out) const override;

            };

            class includes : public element_base {
            private:
                vector < string > _files;
            public:
                
                includes ( const vector < string > & files );

                inline void push_back (const string & file) {
                    _files.push_back (file);
                }

                virtual void write (output & out) const override;

            };

            class cpp_namespace : public group {
            public:

                using shared = shared_ptr < cpp_namespace >;

                namespace_info info;

                cpp_namespace (namespace_info & info);

                virtual void write (output & out) const override;

            };

        }

    }
}

#endif