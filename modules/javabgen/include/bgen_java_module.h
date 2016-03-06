#pragma once
#ifndef	_bgen_java_module_h_
#define _bgen_java_module_h_

#include <bgen_core.h>

namespace bgen {

	class java_plugin : public base_language_plugin {
	private:

		virtual string handled_language() const override;
		virtual bool handles_language(const std::string & lang) const override;

		virtual void generate_host (type_map & types) const override;
		virtual void generate_client (type_map & types) const override;

	};

}

#endif