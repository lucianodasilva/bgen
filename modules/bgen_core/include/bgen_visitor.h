#pragma once
#ifndef _bgen_visitor_h_
#define _bgen_visitor_h_

#include "bgen_dependency.h"
#include "bgen_logger.h"
#include "bgen_struct_info.h"
#include "bgen_type_info.h"
#include "bgen_code_map.h"
#include "bgen_plugin.h"

#include <string>
#include <map>
#include <vector>

using namespace std;

namespace bgen {

	struct visitor_context {
	public:

		code_map		symbols;

		struct_info *   active_struct { nullptr };
		method_info		active_method;
		namespace_info	active_namespace;

		string			active_source_file;

		map < string, dependency >
				active_dependencies;
	};

	class visitor {
	private:

		struct internal;

	public:

		static code_map parse();

	};
	
}

#endif