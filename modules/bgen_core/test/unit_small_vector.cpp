//
// Created by Luciano da Silva on 05/04/17.
//

#include <gtest/gtest.h>
#include <bgen_core.h>

using namespace std;
using namespace bgen;

namespace bgen {
    namespace tests {

        struct unit_small_vector_test : public ::testing::Test {
        protected:
            virtual void SetUp() {}

        public:

            const size_type small_vector_size = 10;


        };

        TEST(unit_small_vector_test, ctor_1) {
            small_vector < 10 > inst;
            EXPECT_EQ(inst.capacity (), small_vector_size);
        }
    }

    }
}