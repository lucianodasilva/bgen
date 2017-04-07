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

			// object move / copy counters
			static size_t s_copy_count = 0;
			static size_t s_move_count = 0;

			// structures to be moved
			struct move_obj {

				size_t & move_counter;
				size_t & copy_counter;

				move_obj (size_t & m_counter, size_t & c_counter) :
					move_counter (m_counter),
					copy_counter (c_counter) {}

				move_obj ( move_obj && m ) :
					move_counter (m.move_counter),
					copy_counter (m.copy_counter)
				{
					++move_counter;
				}

				move_obj (const move_obj & m) :
					move_counter (m.move_counter),
					copy_counter (m.copy_counter)
				{
					++copy_counter;
				}

				move_obj & operator = (const move_obj & m) {
					move_counter = m.move_counter;
					copy_counter = m.copy_counter;
					return *this;
				}

			};

			small_vector<move_obj, unit_small_vector_test::test_size>
					source;

			for (int i = 0; i < unit_small_vector_test::test_size; ++i) {
				source.emplace_back(
					s_move_count,
					s_copy_count
				);
			}

			small_vector < move_obj, unit_small_vector_test::test_size >
				dest = std::move(source);

			EXPECT_EQ (s_copy_count, 0);
			EXPECT_EQ (s_move_count, unit_small_vector_test::test_size);
        }
    }

}