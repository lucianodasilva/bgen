#pragma once
#ifndef _bgen_clang_driver_h_
#define _bgen_clang_driver_h_

#include <bgen_core.h>
#include <memory>
#include <vector>

using namespace std;
using namespace bgen::parser;

namespace bgen {
    namespace clang {

        class driver : public bgen::parser::driver {
        private:

            vector < unique_ptr < native_handle > > _native_handles; 
            element_vector _elements;

        public:

            virtual const element_vector & elements () const override; 

            virtual type get_type (const element & e) const override;
            virtual type get_type (const type & t) const override;

            virtual template_parameter_vector get_template_parameters (const element & e) const override;
            virtual template_parameter_vector get_template_parameters (const type & t) const override;

            virtual semantic_path get_semantic_path (const element & e) const override;
            virtual semantic_path get_semantic_path (const type & t) const override;

            static driver * make_driver (context & cxt);

        }

    }
}

#endif