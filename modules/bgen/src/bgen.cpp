#include "bgen.h"

#include <bgen_core.h>
#include <bgen_config.h>

#include <cmd_args.h>

#include <iostream>
#include <string>

#include "bgen_parameters.h"

using namespace std;
using namespace bgen;

/*
 * TODO: In java packages must correspond to the proper folder
 * structure of the source code. Therefore the file generator
 * must also be a "folder" generator.
 *
 */

void show_version( parameters & params ) {
	cout << "bgen version 0.01" << endl;
}

void show_usage ( parameters & params ) {
	show_version(params);

	auto plugins = bgen::config::get_language_plugins();

	cout << endl << " - avaliable plugins:" << endl;

	for (auto it = plugins.begin(); it != plugins.end(); ++it) {
		if (it != plugins.end ())
			cout << "   " << (*it)->handled_language () << endl;
	}
    
	cout << endl;
    
    cout << "-l (--language): plugin name" << endl;
    cout << "-i (--include): additional includes" << endl;
    cout << "--include_path: include paths" << endl;
    cout << "-s (--source): source files" << endl;
    cout << "-cp (--client-path): path to client output directory" << endl;
    cout << "-hd (--host-path): path to host output directory" << endl;
    cout << "-ca (--compile-args): required compile arguments" << endl;
    
    cout << endl;
    
    cout << "-v (--version): application version number" << endl;
    cout << "-h (--help): this information" << endl;
}

bgen::base_language_plugin * get_language_plugin () {
    
	auto plugins = bgen::config::get_language_plugins();
    auto & args = parameters::get ();

	for (auto * p : plugins) {
		if (p->handles_language(args.language))
			return p;
	}

	cout << "error: unavailable language plugin for \"" << args.language << "\"" << endl;
    
	return nullptr;
}

void process(parameters & params) {
	auto plugin = get_language_plugin ();
    
    if (!plugin)
        return;

    auto types = type_map ();
    
    auto state = visitor::parse( plugin, types);
    
    if (state != success_type::failure)
        plugin->generate (types);
}

int main(int arg_c, char * arg_v[]) {

    /*
	// demo
    auto & demo_params = parameters::get ();
    demo_params.language = "casablanca";
     
    // testing
    // demo_params.source_files.push_back("Z:\\dev\\clang_parser\\generator\\generator\\Test.h");
    // stl
    // demo_params.include_paths.push_back("C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\include");
     
    demo_params.include_paths.push_back("/Library/Developer/CommandLineTools/usr/include/c++/v1/");
    demo_params.include_paths.push_back("/Library/Developer/CommandLineTools/usr/lib/clang/7.0.2/include");
     
    demo_params.source_files.push_back ("/Users/lucianosilva/dev/casablanca-demo.git/src/services.h");
     
    demo_params.client_dest = "/Users/lucianosilva/dev/bgen.git/build/";
    demo_params.host_dest = "/Users/lucianosilva/dev/bgen.git/build/";
     
	process(demo_params);
	return 0;
     */
    
	using namespace command_line;

	auto main_expression =
		key("-l", "--language")[&parameters::language] >
		*(
			key ("-i", "--include")[&parameters::include_files] |
            key ("--include-path")[&parameters::include_paths] |
			key ("-s", "--source")[&parameters::source_files] |
			key ("-cp", "--client_path")[&parameters::client_dest] |
			key ("-hd", "--host-path")[&parameters::host_dest] |
			key ("-ca", "--compile-args")[&parameters::compile_args]
		);

	auto expression =
		usage(option("-v", "--version"))[&show_version] |
		usage(option("-h", "--help"))[&show_usage] |
		usage(main_expression)[&process]
	;

	auto & params = parameters::get();

	if (!parse (expression, arg_c, arg_v, params)) {
        logger::write () << "unexpected command line arguments found";
        show_usage(params);
	}

	return 0;
}
