//
// Created by Luciano Silva on 17/12/16.
//

#include <gtest/gtest.h>
#include <bgen_core.h>

using namespace std;
using namespace bgen;

namespace bgen {
    namespace tests {

        struct error_status_test: public ::testing::Test {
        protected:
            virtual void SetUp() {
                bgen::error_status::reset ();
            }
        };

        TEST(error_status_test, default_status) {
            EXPECT_EQ(bgen::error_status::status (), error_status_type::success);
        }

        TEST(error_status_test, call_warn) {
            bgen::error_status::warn ();

            EXPECT_EQ(bgen::error_status::status (), error_status_type::warnings);
        }

        TEST(error_status_test, call_fail) {
            bgen::error_status::fail();

            EXPECT_EQ(bgen::error_status::status (), error_status_type::failure);
        }

        TEST(error_status_test, call_multiple) {
            bgen::error_status::fail();
            bgen::error_status::warn ();

            EXPECT_EQ(bgen::error_status::status (), error_status_type::failure);
        }

    }
}