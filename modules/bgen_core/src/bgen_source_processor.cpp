#include "bgen_source_processor.h"

namespace bgen {
    namespace source {

        processor::processor (owner < parser * > parser_instance, const plugin_vector & plugins) :
            _parser (parser_instance),
            _plugins (plugins)
        {}

        code_map processor::process (context & cxt) {

            if (!_parser) {
                cxt.status.fail() << "invalid parser";
                return {};
            }

            parser_result parser_info = _parser->parse (cxt);

            if (cxt.status.current_state() == state_type::failure) {
                return {};
            }

            // copy preliminary information
            code_map running_map = {
                parser_info.types,
                parser_info.structs
            };

            // post process information into code code_map


            // run generation plugins
            for (auto & plug : _plugins)
                plug->generate (running_map);
        }

    }
}