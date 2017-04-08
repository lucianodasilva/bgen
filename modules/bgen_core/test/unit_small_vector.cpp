//
// Created by Luciano da Silva on 05/04/17.
//

#include <gtest/gtest.h>
#include <bgen_core.h>
#include <memory>

#include <vector>

using namespace std;
using namespace bgen;

#ifdef _MSC_VER
#define no_inline __declspec(noinline)
#else
#define no_inline __attribute__((__noinline__))
#endif

namespace bgen {
    namespace tests {

        struct unit_small_vector_test : public ::testing::Test {
        protected:
            virtual void SetUp() {}

        public:

            static const size_t test_size;

			template < class _t >
			static no_inline void transfer (_t && v1, _t & v2) {
				v2 = std::move (v1);
			}

        };

        const size_t unit_small_vector_test::test_size = 10;

        TEST(unit_small_vector_test, ctor_default) {
            small_vector < int, unit_small_vector_test::test_size > victim;
            EXPECT_EQ(victim.capacity (), unit_small_vector_test::test_size);
        }

        TEST(unit_small_vector_test, ctor_first_last) {

            vector < int > expectancy = {
                1, 3, 5, 8, 13
            };

            small_vector < int, unit_small_vector_test::test_size >
                victim (expectancy.begin(), expectancy.end());

            EXPECT_EQ(victim.capacity (), unit_small_vector_test::test_size);

            EXPECT_TRUE(std::equal(
                expectancy.begin(), expectancy.end(),
                victim.begin(), victim.end()
            ));
        }

		TEST(unit_small_vector_test, operator_equal_vector) {

			small_vector < int, 5 > expectancy = {
				1, 3, 5, 8, 13
			};

			small_vector < int, unit_small_vector_test::test_size > victim;

			victim.operator = (expectancy);

			EXPECT_EQ(victim.capacity (), unit_small_vector_test::test_size);

			EXPECT_TRUE(std::equal(
				expectancy.begin(), expectancy.end(),
				victim.begin(), victim.end()
			));
		}

		TEST(unit_small_vector_test, operator_equal_vector_grow) {

			const size_t double_size = unit_small_vector_test::test_size * 2;

			small_vector < int, double_size > expectancy;

			for (size_t i = 0; i < double_size; ++i) {
				expectancy.push_back(i);
			}

			small_vector < int, unit_small_vector_test::test_size > victim;

			victim.operator = (expectancy);

			EXPECT_EQ(victim.capacity (), double_size);

			EXPECT_TRUE(std::equal(
				expectancy.begin(), expectancy.end(),
				victim.begin(), victim.end()
			));
		}

        TEST(unit_small_vector_test, ctor_copy) {

            small_vector < int, unit_small_vector_test::test_size >
                expectancy = { 1, 2, 3, 4, 5 };

            small_vector < int, unit_small_vector_test::test_size >
                victim = expectancy;

            EXPECT_EQ(victim.capacity (), unit_small_vector_test::test_size);

            EXPECT_TRUE(std::equal(
                expectancy.begin(), expectancy.end(),
                victim.begin(), victim.end()
            ));
        }

        TEST(unit_small_vector_test, ctor_move) {

			// object move / copy counters
			size_t s_copy_count = 0;
			size_t s_move_count = 0;

			// structures to be moved
			struct move_obj {

				size_t * move_counter;
				size_t * copy_counter;

				move_obj (size_t & m_counter, size_t & c_counter) :
					move_counter (&m_counter),
					copy_counter (&c_counter) {}

				move_obj ( move_obj && m ) :
					move_counter (m.move_counter),
					copy_counter (m.copy_counter)
				{
					++(*move_counter);
				}

				move_obj (const move_obj & m) :
					move_counter (m.move_counter),
					copy_counter (m.copy_counter)
				{
					++(*copy_counter);
				}

				move_obj & operator = (const move_obj & m) {
					move_counter = m.move_counter;
					copy_counter = m.copy_counter;

					++(*copy_counter);
					return *this;
				}

				move_obj & operator = (move_obj && m) {
					std::swap(move_counter, m.move_counter);
					std::swap(copy_counter, m.copy_counter);

					++(*move_counter);
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

			small_vector < move_obj, unit_small_vector_test::test_size > dest = std::move(source);

			EXPECT_EQ (s_copy_count, 0);
			EXPECT_EQ (s_move_count, unit_small_vector_test::test_size);
			EXPECT_EQ (dest.size(), unit_small_vector_test::test_size);
        }

		TEST(unit_small_vector_test, operator_equal_move) {

			// object move / copy counters
			size_t s_copy_count = 0;
			size_t s_move_count = 0;

			// structures to be moved
			struct move_obj {

				size_t * move_counter;
				size_t * copy_counter;

				move_obj (size_t & m_counter, size_t & c_counter) :
					move_counter (&m_counter),
					copy_counter (&c_counter) {}

				move_obj ( move_obj && m ) :
					move_counter (m.move_counter),
					copy_counter (m.copy_counter)
				{
					++(*move_counter);
				}

				move_obj (const move_obj & m) :
					move_counter (m.move_counter),
					copy_counter (m.copy_counter)
				{
					++(*copy_counter);
				}

				move_obj & operator = (const move_obj & m) {
					move_counter = m.move_counter;
					copy_counter = m.copy_counter;

					++(*copy_counter);
					return *this;
				}

				move_obj & operator = (move_obj && m) {
					std::swap(move_counter, m.move_counter);
					std::swap(copy_counter, m.copy_counter);

					++(*move_counter);
					return *this;
				}

			};

			const size_t half_size = unit_small_vector_test::test_size / 2;

			small_vector<move_obj, half_size>
				source;

			for (int i = 0; i < half_size; ++i) {
				source.emplace_back(
					s_move_count,
					s_copy_count
				);
			}

			small_vector < move_obj, unit_small_vector_test::test_size > dest;

			dest.operator=(std::move(source));

			EXPECT_EQ (s_copy_count, 0);
			EXPECT_EQ (s_move_count, half_size);
			EXPECT_EQ (dest.size(), half_size);
			EXPECT_EQ (source.size(), 0);

		}

		TEST(unit_small_vector_test, ctor_il) {

			std::initializer_list < int > expectancy = { 1, 2, 3, 4, 5, 6, 7 ,8 ,9, 10 };

			small_vector < int, unit_small_vector_test::test_size >
				victim = expectancy;

			EXPECT_EQ(victim.capacity (), unit_small_vector_test::test_size);

			EXPECT_TRUE(std::equal(
				expectancy.begin(), expectancy.end(),
				victim.begin(), victim.end()
			));
		}

		TEST(unit_small_vector_test, operator_equals_il) {

			std::initializer_list < int > expectancy = { 1, 2, 3, 4, 5, 6, 7 ,8 ,9, 10 };

			small_vector < int, unit_small_vector_test::test_size >
				victim;

			victim = expectancy;

			EXPECT_EQ(victim.capacity (), unit_small_vector_test::test_size);

			EXPECT_TRUE(std::equal(
				expectancy.begin(), expectancy.end(),
				victim.begin(), victim.end()
			));
		}

		TEST(unit_small_vector_test, dctor) {

			bool was_destroyed = false;

			struct victim_t {

				bool * was_destroyed_ptr = nullptr;

				~victim_t(){
					if (was_destroyed_ptr)
						(*was_destroyed_ptr) = true;
				}

			};

			{
				small_vector < victim_t, 10 > small_v;
				small_v.push_back ({&was_destroyed});
			}

			EXPECT_TRUE(was_destroyed);
		}

		TEST(unit_small_vector_test, dctor_pointer) {
			bool was_destroyed = false;

			struct victim_t {

				bool * was_destroyed_ptr = nullptr;

				victim_t () = default;
				victim_t (bool * destroyed_ptr ) : was_destroyed_ptr (destroyed_ptr) {}

				~victim_t(){
					if (was_destroyed_ptr)
						(*was_destroyed_ptr) = true;
				}

			};

			auto victim = make_unique < victim_t > (&was_destroyed);

			{
				small_vector < victim_t *, 10 > small_v;
				small_v.push_back (victim.get());
			}

			EXPECT_FALSE(was_destroyed);

		}

		TEST(unit_small_vector_test, assign_count_value) {
			const int expected_value = 12345;

			small_vector < int, unit_small_vector_test::test_size > victim;

			victim.assign(unit_small_vector_test::test_size, expected_value);
			EXPECT_EQ(
				std::count (victim.begin(), victim.end(), expected_value),
				unit_small_vector_test::test_size
			);
		}

		TEST(unit_small_vector_test, assign_iterators) {

			small_vector < int, unit_small_vector_test::test_size > expectancy;

			for (size_t i = 0; i < unit_small_vector_test::test_size; ++i) {
				expectancy.push_back(i);
			}

			small_vector < int, unit_small_vector_test::test_size >
				victim;

			victim.assign(expectancy.begin (), expectancy.end());

			EXPECT_TRUE(std::equal(
				expectancy.begin(), expectancy.end(),
				victim.begin(), victim.end()
			));
		}

		TEST(unit_small_vector_test, assign_il) {

			std::initializer_list < int > expectancy = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};


			small_vector < int, unit_small_vector_test::test_size >
				victim;

			victim.assign(expectancy);

			EXPECT_TRUE(std::equal(
				expectancy.begin(), expectancy.end(),
				victim.begin(), victim.end()
			));
		}

		TEST(unit_small_vector_test, at_within_range) {
			small_vector < int, unit_small_vector_test::test_size >
				victim = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

			EXPECT_EQ (victim.at (5), 5);
		}

		TEST(unit_small_vector_test, at_out_of_range) {
			small_vector < int, unit_small_vector_test::test_size >
				victim = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

			ASSERT_THROW (victim.at (11), std::out_of_range);
		}

		TEST(unit_small_vector_test, operator_index) {
			small_vector < int, unit_small_vector_test::test_size >
				victim = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

			EXPECT_EQ (victim[5], 5);
		}

		TEST(unit_small_vector_test, operator_index_edit) {
			small_vector < int, unit_small_vector_test::test_size >
				victim = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

			victim [5] = 11;

			EXPECT_EQ (victim[5], 11);
		}

		TEST(unit_small_vector_test, front) {
			small_vector < int, unit_small_vector_test::test_size >
				victim = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

			EXPECT_EQ (victim.front(), 0);
		}

		TEST(unit_small_vector_test, front_edit) {
			small_vector < int, unit_small_vector_test::test_size >
				victim = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

			victim.front() = 11;

			EXPECT_EQ (victim.front(), 11);
		}

		TEST(unit_small_vector_test, front_const) {
			const small_vector < int, unit_small_vector_test::test_size >
				victim = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

			EXPECT_EQ (victim.front(), 0);
		}

		TEST(unit_small_vector_test, back) {
			small_vector < int, unit_small_vector_test::test_size >
				victim = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

			EXPECT_EQ (victim.back(), 9);
		}

		TEST(unit_small_vector_test, back_edit) {
			small_vector < int, unit_small_vector_test::test_size >
				victim = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

			victim.back() = 11;

			EXPECT_EQ (victim.back(), 11);
		}

		TEST(unit_small_vector_test, back_const) {
			const small_vector < int, unit_small_vector_test::test_size >
				victim = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

			EXPECT_EQ (victim.back(), 9);
		}

		TEST(unit_small_vector_test, data) {
			small_vector < int, unit_small_vector_test::test_size >
				victim = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

			EXPECT_EQ (victim.data()[5], 5);
		}

		TEST(unit_small_vector_test, data_const) {
			const small_vector < int, unit_small_vector_test::test_size >
				victim = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

			EXPECT_EQ (victim.data()[5], 5);
		}

		TEST(unit_small_vector_test, empty) {
			const small_vector < int, unit_small_vector_test::test_size >
				victim_a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

			const small_vector < int, unit_small_vector_test::test_size >
				victim_b;

			EXPECT_FALSE (victim_a.empty());
			EXPECT_TRUE (victim_b.empty());
		}

		TEST(unit_small_vector_test, size) {
			std::initializer_list < int > expectancy = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

			small_vector < int, unit_small_vector_test::test_size >
				victim = expectancy;

			EXPECT_EQ(victim.size(), expectancy.size());
		}

		TEST(unit_small_vector_test, max_size) {
			small_vector < int, unit_small_vector_test::test_size >
				victim;

			using size_type = typename small_vector < int, 10 >::size_type;

			EXPECT_EQ(
				victim.max_size(),
				std::numeric_limits < size_type >::max()
			);
		}

		TEST(unit_small_vector_test, reserve) {

			small_vector < int, unit_small_vector_test::test_size >
				victim = { 0, 1, 2, 3, 4 };

			const size_t expected_capacity = unit_small_vector_test::test_size * 4;

			size_t size = victim.size();

			victim.reserve (expected_capacity);

			EXPECT_EQ (size, victim.size());
			EXPECT_EQ (expected_capacity, victim.capacity());
		}


		TEST(unit_small_vector_test, shrink_to_fit) {
			small_vector < int, unit_small_vector_test::test_size >
				victim = { 0, 1, 2, 3, 4 };

			size_t size = victim.size();

			victim.reserve (unit_small_vector_test::test_size * 4);
			victim.shrink_to_fit();

			EXPECT_EQ (size, victim.size());
			EXPECT_EQ (size, victim.capacity());
		}

		TEST(unit_small_vector_test, clear) {
			small_vector < int, unit_small_vector_test::test_size >
				victim = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

			size_t capacity = victim.capacity();

			victim.clear();

			EXPECT_EQ (0, victim.size());
			EXPECT_EQ (capacity, victim.capacity());
		}

    }
}