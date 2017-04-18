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

		// structures to be moved
		struct small_vector_move_item {

			size_t * move_counter;
			size_t * copy_counter;

			small_vector_move_item(size_t & m_counter, size_t & c_counter) :
				move_counter(&m_counter),
				copy_counter(&c_counter) {}

			small_vector_move_item(small_vector_move_item && m) :
				move_counter(m.move_counter),
				copy_counter(m.copy_counter)
			{
				++(*move_counter);
			}

			small_vector_move_item(const small_vector_move_item & m) :
				move_counter(m.move_counter),
				copy_counter(m.copy_counter)
			{
				++(*copy_counter);
			}

			small_vector_move_item & operator = (const small_vector_move_item & m) {
				move_counter = m.move_counter;
				copy_counter = m.copy_counter;

				++(*copy_counter);
				return *this;
			}

			small_vector_move_item & operator = (small_vector_move_item && m) {
				std::swap(move_counter, m.move_counter);
				std::swap(copy_counter, m.copy_counter);

				++(*move_counter);
				return *this;
			}

			static inline void init_items(small_vector_base < small_vector_move_item > & victim, size_t item_count, size_t & copy_count, size_t & move_count) {
				for (int i = 0; i < item_count; ++i) {
					victim.emplace_back(
						move_count,
						copy_count
					);
				}
			}

		};

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

        const size_t unit_small_vector_test::test_size = 16;

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

			const size_t double_size = common::next_pow_2(unit_small_vector_test::test_size * 2);

			small_vector < size_t, double_size > expectancy;

			for (size_t i = 0; i < double_size; ++i) {
				expectancy.push_back(i);
			}

			small_vector < size_t, unit_small_vector_test::test_size > victim;

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

			small_vector<small_vector_move_item, unit_small_vector_test::test_size>
					source;

			small_vector_move_item::init_items(
				source,
				unit_small_vector_test::test_size,
				s_copy_count,
				s_move_count
			);

			small_vector < small_vector_move_item, unit_small_vector_test::test_size > dest = std::move(source);

			EXPECT_EQ (s_copy_count, 0);
			EXPECT_EQ (s_move_count, unit_small_vector_test::test_size);
			EXPECT_EQ (dest.size(), unit_small_vector_test::test_size);
        }

		TEST(unit_small_vector_test, operator_equal_move) {

			// object move / copy counters
			size_t s_copy_count = 0;
			size_t s_move_count = 0;

			const size_t half_size = unit_small_vector_test::test_size / 2;

			small_vector<small_vector_move_item, half_size>
				source;

			small_vector_move_item::init_items(
				source,
				half_size,
				s_copy_count,
				s_move_count
			);

			small_vector < small_vector_move_item, unit_small_vector_test::test_size > dest;

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

			small_vector < size_t, unit_small_vector_test::test_size > expectancy;

			for (size_t i = 0; i < unit_small_vector_test::test_size; ++i) {
				expectancy.push_back(i);
			}

			small_vector < size_t, unit_small_vector_test::test_size >
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

			const size_t expected_capacity = common::next_pow_2(unit_small_vector_test::test_size * 4);

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

		TEST(unit_small_vector_test, insert_copy_at_position) {

			int const insert_value = 999;
			int const insert_position = 5;
			int const expected_size = 11;

			small_vector < int, unit_small_vector_test::test_size >
				victim = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

			auto position = victim.insert(victim.begin() + insert_position, insert_value);

			EXPECT_EQ(expected_size, victim.size());
			EXPECT_EQ(insert_value, victim[insert_position]);
			EXPECT_EQ(victim.begin() + insert_position, position);
		}

		TEST(unit_small_vector_test, insert_move_at_position) {

			int const insert_value = 999;
			int const insert_position = 5;
			int const expected_size = unit_small_vector_test::test_size + 1;

			// object move / copy counters
			size_t s_copy_count = 0;
			size_t s_move_count = 0;

			small_vector < small_vector_move_item, expected_size * 2 > victim;

			small_vector_move_item::init_items(
				victim,
				unit_small_vector_test::test_size,
				s_copy_count,
				s_move_count
			);

			// reset copy and move counts
			s_copy_count = 0;
			s_move_count = 0;

			small_vector_move_item move_it{ s_move_count, s_copy_count };

			auto position = victim.insert(victim.begin() + insert_position, std::move (move_it));

			EXPECT_EQ(expected_size, victim.size());
			EXPECT_EQ(s_copy_count, 0);
			EXPECT_EQ(s_move_count, expected_size - insert_position);
			EXPECT_EQ(position, victim.begin() + insert_position);
		}

		TEST(unit_small_vector_test, insert_n_copies_at_position) {

			size_t const insert_value = 1;
			size_t const insert_count = 5;
			size_t const border_count = 4;
			size_t const half_border = border_count / 2;
			size_t const expected_size = insert_count + border_count;
			size_t const border_value = 8;

			small_vector < int, unit_small_vector_test::test_size >
				victim(border_count, border_value);

			auto position = victim.insert(victim.begin() + half_border, insert_count, insert_value);

			EXPECT_EQ(expected_size, victim.size());

			const size_t left_border_count = std::count(
				victim.begin(), victim.begin() + half_border,
				border_value
			);

			EXPECT_EQ(left_border_count, half_border);

			const size_t right_border_count = std::count(
				victim.end() - half_border, victim.end(),
				border_value
			);

			EXPECT_EQ(right_border_count, half_border);

			const size_t inserted_items_count = std::count(
				victim.begin() + half_border, victim.end() - half_border,
				insert_value
			);

			EXPECT_EQ(inserted_items_count, insert_count);

			EXPECT_EQ(position, victim.begin() + half_border);
		}

		TEST(unit_small_vector_test, insert_zero_copies_at_position) {

			size_t const expected_count = 8;
			size_t const expected_value = 8;

			size_t const expected_pos = expected_count / 2;

			small_vector < int, unit_small_vector_test::test_size >
				victim(expected_count, expected_value);

			auto position = victim.insert(victim.begin() + expected_pos, 0, expected_value);

			EXPECT_EQ(victim.size(), expected_count);
			EXPECT_EQ(position, victim.begin() + expected_pos);
		}

		TEST(unit_small_vector_test, insert_position_end) {

			size_t const initial_count = 8;
			size_t const initial_value = 8;
			size_t const insert_count = 3;
			size_t const insert_value = 3;

			size_t const expected_count = initial_count + insert_count;

			small_vector < int, unit_small_vector_test::test_size >
				victim(initial_count, initial_value);

			victim.insert(victim.end(), insert_count, insert_value);

			EXPECT_EQ(expected_count, victim.size());

			EXPECT_EQ(
				std::count(victim.end() - insert_count, victim.end(), insert_value),
				insert_count
			);
		}

		TEST(unit_small_vector_test, insert_position_out_of_range) {

			std::unique_ptr < int > dummy_pos = make_unique < int >(123);

			small_vector < int, unit_small_vector_test::test_size >
				victim(8, 8);

			EXPECT_THROW(victim.insert(dummy_pos.get(), 8, 8), std::out_of_range);
		}

		TEST(unit_small_vector_test, insert_position_span) {

			small_vector < int, unit_small_vector_test::test_size >
				victim = { 0, 1, 2, 3, 4, 5 };

			size_t
				victim_original_size = victim.size(),
				victim_original_half_size = victim_original_size / 2;

			auto insert_pos = victim.begin () + victim_original_half_size;

			small_vector < int, unit_small_vector_test::test_size >
				source = { 10, 11, 12, 13 };

			auto pos = victim.insert(insert_pos, source.begin(), source.end());

			EXPECT_EQ(victim_original_size + source.size(), victim.size());

			EXPECT_TRUE(std::equal(
				pos, pos + source.size(),
				source.begin(), source.end()
			));
		}

		TEST(unit_small_vector_test, insert_position_span_out_of_range) {

			std::unique_ptr < int > dummy_pos = make_unique < int >(123);

			small_vector < int, unit_small_vector_test::test_size >
				victim(8, 8);

			small_vector < int, unit_small_vector_test::test_size >
				source = { 10, 11, 12, 13 };

			EXPECT_THROW(victim.insert(dummy_pos.get(), source.begin(), source.end()), std::out_of_range);
		}

		TEST(unit_small_vector_test, insert_position_init_list) {

			small_vector < int, unit_small_vector_test::test_size >
				victim = { 0, 1, 2, 3, 4, 5 };

			size_t
				victim_original_size = victim.size(),
				victim_original_half_size = victim_original_size / 2;

			auto insert_pos = victim.begin() + victim_original_half_size;

			std::initializer_list < int >
				source = { 10, 11, 12, 13 };

			auto pos = victim.insert(insert_pos, source);

			EXPECT_EQ(victim_original_size + source.size(), victim.size());

			EXPECT_TRUE(std::equal(
				pos, pos + source.size(),
				source.begin(), source.end()
			));
		}

		TEST(unit_small_vector_test, emplace_at_position) {

			int const value_a = 999;
			int const value_b = 666;

			int const insert_position = 5;
			int const expected_size = 11;

			struct test_struct {
				int a;
				int b;

				test_struct() = default;
				test_struct(const test_struct &) = default;
				test_struct (int va, int vb) : a (va), b (vb) {}
			};

			small_vector < test_struct, unit_small_vector_test::test_size >
				victim(10, { 1, 1 });

			auto position = victim.emplace(victim.begin() + insert_position, value_a, value_b);

			EXPECT_EQ(expected_size, victim.size());

			EXPECT_EQ(value_a, victim[insert_position].a);
			EXPECT_EQ(value_b, victim[insert_position].b);

			EXPECT_EQ(victim.begin() + insert_position, position);
		}

		TEST(unit_small_vector_test, erase_at_position) {

			int const expected_size = 9;
			int const erase_index = 5;
			int const erase_value = 5;

			small_vector < int, unit_small_vector_test::test_size >
				victim = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

			auto position = victim.erase(victim.begin() + erase_index);

			EXPECT_EQ(victim.size(), expected_size);
			EXPECT_EQ(find(victim.begin(), victim.end(), erase_value), victim.end());
			EXPECT_EQ(victim.begin() + erase_index, position);
		}

		TEST(unit_small_vector_test, erase_iterator_range) {

			const small_vector < int, unit_small_vector_test::test_size >
				origin = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

			small_vector < int, unit_small_vector_test::test_size >
				victim = origin;

			size_t const erase_offset = 3;
			size_t const expected_size = erase_offset * 2;

			auto position = victim.erase(victim.begin() + erase_offset, victim.end() - erase_offset);

			EXPECT_EQ(victim.size(), expected_size);

			EXPECT_TRUE(std::equal(
				victim.begin(),
				victim.begin() + erase_offset,
				origin.begin(),
				origin.begin() + erase_offset
			));

			EXPECT_TRUE(std::equal(
				victim.end() - erase_offset,
				victim.end(),
				origin.end() - erase_offset,
				origin.end()
			));
		}

    }
}