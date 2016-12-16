//
// Created by Luciano Silva on 15/12/16.
//

#pragma once
#ifndef _bgen_clang_type_maker_h_
#define _bgen_clang_type_maker_h_

#include <clang-c/Index.h>
#include "bgen_visitor.h"

namespace bgen {
    namespace clang {

        type_info * make_type (visitor_context & context, const CXType & native_type);

    }
}

#endif //_bgen_clang_type_maker_h_