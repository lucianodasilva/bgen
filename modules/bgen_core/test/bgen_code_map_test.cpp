//
// Created by Luciano Silva on 17/12/16.
//

#include <gtest/gtest.h>
#include <bgen_core.h>
#include <memory>
#include <string>

using namespace std;

namespace bgen {
    namespace tests {

        template < class _t >
        map < string,unique_ptr < _t > > & get_field (code_map & m);

        template <>
        map < string, unique_ptr < bgen::struct_info > > & get_field < bgen::struct_info > (code_map & m) {
            return m.structs;
        }

        template <>
        map < string, unique_ptr < bgen::type_info > > & get_field < bgen::type_info > (code_map & m) {
            return m.types;
        }

        template < class _t >
        typename map < string, unique_ptr < _t > >::iterator find_item (code_map & m, const string & id) {
            return get_field < _t > (m).find (id);
        };

        template < class _t >
        typename map < string, unique_ptr < _t > >::iterator get_end (code_map & m) {
            return get_field < _t > (m).end ();
        };

        template < class _t >
        _t * get_or_make (code_map & m, const string & id);

        template <>
        bgen::struct_info * get_or_make < bgen::struct_info > (code_map & m, const string & id) {
            return get_or_make_struct(m, id);
        }

        template <>
        bgen::type_info * get_or_make < bgen::type_info > (code_map & m, const string & id) {
            return get_or_make_type(m, id);
        }

        template < class _t >
        struct get_or_make_types_test: public ::testing::Test {
        protected:

            using value_type = _t;

            virtual void SetUp() {}
        };

        using implementations = testing::Types <bgen::struct_info, bgen::type_info>;

        TYPED_TEST_CASE(get_or_make_types_test, implementations);

        TYPED_TEST(get_or_make_types_test, new_type) {

            const string type_name = "new_type";
            bgen::code_map map;

            auto nt = get_or_make < TypeParam > (map, type_name);

            EXPECT_NE(nullptr, nt) << "no new type returned";

            auto it = find_item < TypeParam > (map, type_name);

            if (it == get_end < TypeParam > (map))
                FAIL() << "no new type created";
        }

        TYPED_TEST(get_or_make_types_test, get_existing_type) {

            // prepare test
            const string type_name = "new_type";
            bgen::code_map map;

            auto nt = get_or_make < TypeParam > (map, type_name);

            EXPECT_NE(nullptr, nt) << "no new type returned";

            auto it = find_item < TypeParam > (map, type_name);

            if (it == get_end < TypeParam > (map))
                FAIL() << "no new type created";

            // run test
            auto again = get_or_make < TypeParam > (map, type_name);

            auto again_it = find_item < TypeParam > (map, type_name);

            if (again_it == get_end < TypeParam > (map))
                FAIL() << "no type returned!";
        }

    }
}