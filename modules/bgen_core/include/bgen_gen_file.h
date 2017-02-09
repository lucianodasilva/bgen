#pragma once
#ifndef _bgen_gen_file_h_
#define _bgen_gen_file_h_

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "bgen_gen_elements.h"

using namespace std;

namespace bgen {
    namespace gen {

        class file : public group {
        private:

            string _filename;

        public:

            using shared = shared_ptr < file >;

            file (const string & filename);

            void write (context & cxt) const;

            virtual void write (context & cxt, output & out) const override;


        };

    }
}

#endif