#pragma once
#ifndef _bgen_visitor_h_
#define _bgen_visitor_h_

#include <string>
#include <map>
#include <vector>

#include "bgen_source_code_map.h"

using namespace std;

namespace bgen {

	class visitor {
	private:

		struct internal;

	public:

		static source::code_map parse();

	};
	
}

#endif