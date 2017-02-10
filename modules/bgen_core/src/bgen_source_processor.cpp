#include "bgen_source_processor.h"

namespace bgen {
    namespace source {

        code_map process (parser * parser_instance, context & cxt) {

            if (!parser_instance) {
                cxt.status.fail() << "invalid parser";
                return {};
            }

            parser_result parser_info = parser_instance->parse (cxt);

            if (cxt.status.current_state() == state_type::failure) {
                return {};
            }

            // copy preliminary information
            code_map running_map = {
                parser_info.types,
                parser_info.structs
            };

            // post process information into code code_map

            return {};
        }

    }
}