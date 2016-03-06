#pragma once
#ifndef _bgen_visitor_h_
#define _bgen_visitor_h_

#include "bgen_dependency.h"
#include "bgen_logger.h"
#include "bgen_struct_info.h"
#include "bgen_type_info.h"
#include "bgen_type_map.h"
#include "bgen_plugin.h"

#include <string>
#include <map>
#include <vector>

using namespace std;

namespace bgen {

	class visitor {
	private:

		struct internal;

		struct context {
        public:
            
			type_map			types;

			struct_info::shared	active_struct;
			method_info			active_method;
			namespace_info		active_namespace;

			string				active_source_file;
            
            map < string, dependency > 
                                active_dependencies;
            
        private:
            success_type        _state;
        public:
            
            inline success_type state () { return _state; }
            
            inline void warn () { if (_state != success_type::failure) _state = success_type::warnings; }
            
            inline void fail () { _state = success_type::failure; }
		};

	public:

		static success_type parse(
			base_language_plugin * plugin, type_map & out_map
		);

	};
	
}

#endif