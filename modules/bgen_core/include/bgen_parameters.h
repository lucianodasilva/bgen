#pragma once

#ifndef	_bgen_parameters_h_
#define _bgen_parameters_h_

#include <string>
#include <vector>
#include "bgen_details.h"

using namespace std;

namespace bgen {

	struct parameters : no_copy {

		vector < string >	plugins;
		string				compile_args;

		string				client_dest;
		string				host_dest;

		vector < string >	source_files;
		vector < string >	include_files;
		vector < string >	include_paths;
        
        bool                no_default_includes;

		static parameters & get();


	};

}

#endif