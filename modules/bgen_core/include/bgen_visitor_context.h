//
// Created by Luciano Silva on 18/12/16.
//

#pragma once
#ifndef _bgen_visitor_context_h_
#define _bgen_visitor_context_h_

#include "bgen_clang_handlers.h"
#include "bgen_code_map.h"
#include "bgen_struct_info.h"
#include "bgen_type_info.h"

#include <memory>
#include <string>
#include <unordered_map>

using namespace std;

namespace bgen {

    using dependency_map = unordered_map < string, dependency >;

    struct visitor_context {
        code_map		symbols;

        dependency_map  active_dependencies;

        const clang::handlers::lookup
                        handler_lookup { clang::handlers::lookup_make_default () };

        method_info		active_method;
        namespace_info	active_namespace;

        string			active_source_file;

        struct_info *   active_struct { nullptr };
    };
}

#endif //_bgen_visitor_context_h_
