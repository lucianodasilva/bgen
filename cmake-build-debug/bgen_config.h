#pragma once
#ifndef _bgen_config_h_in_
#define _bgen_config_h_in_

#include <bgen.h>
#include <vector>

using namespace std;

namespace bgen {

	

	struct config : no_copy {
	public:
		
		static inline const vector < base_plugin * > & get_plugins () {
			static const plugin_vector plugins = {
				
			};
			
			return plugins;
		}
		
	};

}

#endif
