#pragma once

#ifndef	_bgen_source_processor_h_
#define _bgen_source_processor_h_

#include "bgen_source_code_map.h"
#include "bgen_source_parser.h"
#include "bgen_plugin.h"

#include "bgen_details.h"

#include <memory>

using namespace std;

namespace bgen {
	namespace source {

		class processor {
		private:
			unique_ptr < parser > 	_parser;
			plugin_vector 			_plugins;
		public:
			processor (owner < parser * > parser_instance, const plugin_vector & plugins);
			code_map process (context & cxt);
		};
	}
}

#endif