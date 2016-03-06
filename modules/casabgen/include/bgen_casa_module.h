#pragma once
#ifndef	_bgen_casa_module_h_
#define _bgen_casa_module_h_

#include <bgen_core.h>

namespace bgen {

	class casablanca_plugin : public base_language_plugin {
	private:

		virtual string handled_language() const override;
		virtual bool handles_language(const std::string & lang) const override;

		virtual void generate (type_map & types) const override;

	};

}

#endif