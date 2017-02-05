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

	class visitor {
	private:

		struct internal;

	public:

		static code_map parse();

	};
	
}

#endif