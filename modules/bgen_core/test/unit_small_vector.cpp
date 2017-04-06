//
// Created by Luciano da Silva on 05/04/17.
//

#include <gtest/gtest.h>
#include <bgen_core.h>

#include <vector>

using namespace std;
using namespace bgen;

namespace bgen {
    namespace tests {

        struct unit_small_vector_test : public ::testing::Test {
        protected:
            virtual void SetUp() {}

        public:

            static const size_t test_size;


        };

        const size_t unit_small_vector_test::test_size = 10;

        TEST(unit_small_vector_test, ctor_default) {
            small_vector < int, unit_small_vector_test::test_size > inst;
            EXPECT_EQ(inst.capacity (), unit_small_vector_test::test_size);
        }

        TEST(unit_small_vector_test, ctor_first_last) {

            vector < int > expectancy = {
                1, 3, 5, 8, 13
            };

            small_vector < int, unit_small_vector_test::test_size >
                inst (expectancy.begin(), expectancy.end());

            EXPECT_EQ(inst.capacity (), unit_small_vector_test::test_size);

            EXPECT_TRUE(std::equal(
                expectancy.begin(), expectancy.end(),
                inst.begin(), inst.end()
            ));
        }

        TEST(unit_small_vector_test, ctor_copy) {

            small_vector < int, unit_small_vector_test::test_size >
                expectancy = { 1, 2, 3, 4, 5 };

            small_vector < int, unit_small_vector_test::test_size >
                inst = expectancy;

            EXPECT_EQ(inst.capacity (), unit_small_vector_test::test_size);

            EXPECT_TRUE(std::equal(
                expectancy.begin(), expectancy.end(),
                inst.begin(), inst.end()
            ));
        }

        TEST(unit_small_vector_test, ctor_move) {

        }
    }

}