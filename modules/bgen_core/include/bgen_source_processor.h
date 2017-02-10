#pragma once

#ifndef	_bgen_source_processor_h_
#define _bgen_source_processor_h_

#include "bgen_source_code_map.h"
#include "bgen_source_parser.h"

#include "bgen_details.h"

#include <memory>

using namespace std;

namespace bgen {
	namespace source {
		code_map process (parser * parser_instance, context & cxt);
	}
}

#endif