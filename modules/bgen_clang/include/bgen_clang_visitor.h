#pragma once
#ifndef _bgen_clang_visitor_h_
#define _bgen_clang_visitor_h_

#include <bgen_core.h>
#include "bgen_clang_tools.h"

namespace bgen {
    namespace clang {
        namespace visitor {

            struct visitor_context {
                context & cxt;
                parser_result & result;
            };

            bool is_valid_kind(type_kind kind);

            size_t make_type (visitor_context & visitor_cxt, const CXType & native_type);

            void handle_inplace_struct (visitor_context & visitor_cxt, const CXType & src_type, bgen::source::type & type);

            size_t make_type (visitor_context & visitor_cxt, const CXType & native_type);

            CXChildVisitResult visit_child (
                CXCursor cursor,
                CXCursor parent,
                CXClientData client_data
            );
                
        }
    }
}

#endif