//
// Created by Luciano Silva on 17/12/16.
//

#include <gtest/gtest.h>
#include <bgen_core.h>
#include <bgen_clang_handlers.h>

using namespace std;
using namespace bgen;
using namespace bgen::clang;

namespace bgen {
    namespace tests {

        struct handler_lookup_test: public ::testing::Test {
        protected:
            virtual void SetUp() {}
        };

        TEST(handler_lookup_test, get_handler_match) {
            auto handler = &clang::handlers::lookup::get (CXCursorKind::CXCursor_ParmDecl);

            EXPECT_NE(nullptr, dynamic_cast < handlers::param_handler * > (handler));
        }

        TEST(handler_lookup_test, get_handler_no_match) {
            auto handler = &handlers::lookup::get (CXCursorKind::CXCursor_AsmStmt);

            EXPECT_NE(nullptr, dynamic_cast < handlers::cursor_type_handler * > (handler));
        }
    }
}