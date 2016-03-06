#pragma once
#ifndef _bgen_java_client_generator_h_
#define _bgen_java_client_generator_h_

#include <bgen_core.h>

namespace bgen {
    namespace gen {

        struct java_helper {

            static void write_visibility (
                    visibility_type visibility,
                    ostream & stream
            );

            static void write_package (
                    const namespace_info & package,
                    ostream & stream
            );

            static bool write_basic_type (
                    type_kind kind,
                    ostream &stream
            );

            static bool write_array (
                    type_info::shared type,
                    const string & identifier,
                    ostream & stream
            );

            static bool write_type (
                    const type_info::shared & type,
                    const string & identifier,
                    ostream & stream
            );
        };

        class java_package : public element_base {
        private:
            namespace_info _namespace;
        public:

            using shared = shared_ptr < java_package >;

            java_package ( const namespace_info & namespace_name);

            virtual void write (ostream & stream) const override;
        };

        class java_field : public element_base {
        private:
            field_info _field;
        public:

            using shared = shared_ptr < java_field >;

            java_field ( const field_info & field_v );

            virtual void write (ostream & stream) const override;
        };

        // TODO: RETHINK INTERMEDIATE MODEL

    }
}

#endif