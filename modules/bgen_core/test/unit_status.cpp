//
// Created by Luciano Silva on 17/12/16.
//

#include <gtest/gtest.h>
#include <bgen_core.h>

#include <sstream>

using namespace std;
using namespace bgen;

namespace bgen {
    namespace tests {

        struct status_test: public ::testing::Test {
        protected:
            virtual void SetUp() {}
        public:

            struct expectancy_builder {

                stringstream out;

                inline expectancy_builder add_line (const string & v) {
                    out << v << endl;
                    return { move (out) };
                }

                inline expectancy_builder add_line (const string & v, const bgen::parser::location & location) {
                    out << v << " @" << location.file << ":" << location.line<< ":" << location.column << endl;
                    return { move (out) };
                }

                inline operator string () const {
                    return out.str ();
                }

            };

            inline static expectancy_builder make_expectancy () {
                return expectancy_builder ();
            }

        };

        TEST(status_test, default_status) {
            stringstream out;
            bgen::status status (out);

            EXPECT_EQ(status.current_state(), state_type::success);
            EXPECT_EQ(out.str (), string());
        }

        TEST(status_test, call_warn) {
            stringstream out;
            bgen::status status (out);

            const string WARNING_MESSAGE = "WARNING";

            status.warn () << WARNING_MESSAGE;

            string expectancy = status_test::make_expectancy().add_line(WARNING_MESSAGE);

            EXPECT_EQ(status.current_state(), state_type::warnings);
            EXPECT_EQ(out.str (), expectancy);
        }

        TEST(status_test, call_warn_with_location) {
            stringstream out;
            bgen::status status (out);

            bgen::parser::location loc = {
                "xpto.h",
                123,
                456
            };

            const string WARNING_MESSAGE = "WARNING";
            const string expectancy = status_test::make_expectancy()
                    .add_line (WARNING_MESSAGE, loc);

            status.warn (loc) << WARNING_MESSAGE;

            EXPECT_EQ(status.current_state(), state_type::warnings);
            EXPECT_EQ(out.str (), expectancy);
        }

        TEST(status_test, call_fail) {
            stringstream out;
            bgen::status status (out);

            const string FAIL_MESSAGE = "FAIL";
            const string expectancy = status_test::make_expectancy()
                    .add_line(FAIL_MESSAGE);

            status.fail () << FAIL_MESSAGE;

            EXPECT_EQ(status.current_state(), state_type::failure);
            EXPECT_EQ(out.str (), expectancy);
        }

        TEST(status_test, call_info) {
            stringstream out;
            bgen::status status (out);

            const string INFO_MESSAGE = "INFO";

            const string expectancy = status_test::make_expectancy()
                    .add_line(INFO_MESSAGE);

            status.info () << INFO_MESSAGE;

            EXPECT_EQ(status.current_state(), state_type::success);
            EXPECT_EQ(out.str (), expectancy);
        }

        TEST(status_test, call_warn_after_fail) {
            stringstream out;
            bgen::status status (out);

            const string WARNING_MESSAGE = "WARNING";
            const string FAIL_MESSAGE = "FAIL";

            const string expectancy = status_test::make_expectancy()
                    .add_line(FAIL_MESSAGE)
                    .add_line(WARNING_MESSAGE);

            status.fail () << FAIL_MESSAGE;
            status.warn () << WARNING_MESSAGE;

            EXPECT_EQ(status.current_state(), state_type::failure);
            EXPECT_EQ(out.str (), expectancy);
        }

    }
}