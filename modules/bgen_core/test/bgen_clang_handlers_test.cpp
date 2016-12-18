//
// Created by Luciano Silva on 17/12/16.
//

#include <gtest/gtest.h>
#include <bgen_core.h>
#include <bgen_clang_handlers.h>

using namespace std;
using namespace bgen;
using namespace bgen::clang::handlers;

namespace bgen {
    namespace tests {

		const CXCursorKind unused_cursor_kind { CXCursorKind::CXCursor_WhileStmt };

		struct handler_lookup_test: public ::testing::Test {
        protected:

            virtual void SetUp() {}

        };

		class dummy_handler : public cursor_type_handler {};

        TEST(handler_lookup_test, get_handler_match) {
			auto lookup = lookup_make_default ();

			cursor_type_handler * new_handler = new dummy_handler();

			lookup.handlers [unused_cursor_kind] = unique_ptr < cursor_type_handler > (new_handler);

			auto handler = &lookup_get (lookup, unused_cursor_kind);

			EXPECT_NE(nullptr, handler);
			EXPECT_EQ(new_handler, handler);
        }

        TEST(handler_lookup_test, get_handler_no_match) {
			auto lookup = lookup_make_default ();

			auto handler = &lookup_get (lookup, unused_cursor_kind);

			EXPECT_NE(nullptr, handler);
			EXPECT_EQ(lookup.null_handler.get (), handler);
        }

		struct handler_types_test: public ::testing::Test {
		protected:

			virtual void SetUp() {}

		};

		// handlers are untestable at this point
    }
}