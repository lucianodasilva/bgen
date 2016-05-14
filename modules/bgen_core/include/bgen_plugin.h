#pragma once
#ifndef	_bgen_plugin_h_
#define _bgen_plugin_h_

#include "bgen_details.h"

#include <vector>
#include <memory>
using namespace std;

namespace bgen {

	struct code_map;

	class base_language_plugin {
	public:

		virtual ~base_language_plugin();

		virtual string handled_language() const = 0;
		virtual bool handles_language (const std::string & lang) const = 0;

		virtual void generate (const code_map & types) const = 0;

	};

	using plugin_vector = vector < base_language_plugin * >;

}

#endif