#pragma once
#ifndef	_bgen_plugin_h_
#define _bgen_plugin_h_

#include "bgen_details.h"

#include <vector>
#include <memory>
using namespace std;

namespace bgen {

	struct code_map;

	class base_plugin {
	public:

		virtual ~base_plugin();

		virtual const string & public_name() const = 0;
		virtual bool accepts (const std::string & name) const = 0;

		virtual void generate (const code_map & types) const = 0;

	};

	using plugin_vector = vector < base_plugin * >;

}

#endif