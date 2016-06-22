#pragma once
#ifndef	_bgen_casa_module_h_
#define _bgen_casa_module_h_

#include <bgen_core.h>

namespace bgen {

	class casablanca_plugin : public base_plugin {
    public:

		virtual const string & public_name () const override;
		virtual bool accepts (const std::string & name) const override;
		virtual void generate (const code_map & symbols) const override;

	};

}

#endif