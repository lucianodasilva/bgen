#include "bgen_parser_visitor.h"
#include "bgen_parser_cursor.h"

#include <functional>
#include <unordered_map>
#include <utility>

namespace bgen {
    namespace parser {

        void visitor::begin_visit (context & cxt) {
            cxt.struct_stack.emplace_back ();

            // add a "root" structure to represent global fields and methods;
            source::struct_info & global = cxt.struct_stack.back ();

            global.identifier = "[GLOBAL]"; 
        }

        void visitor::visit (context & cxt) {
            cxt.interpreter.execute (cxt.cursor.kind, cxt, cxt.cursor);
        }

        void visitor::end_visit (context & cxt) {

        }

    }
}