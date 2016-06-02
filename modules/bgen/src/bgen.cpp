#include "bgen.h"

#include <bgen_core.h>
#include <bgen_config.h>

#include <cmd_args.h>

#include <iostream>
#include <string>

#include "bgen_error_status.h"
#include "bgen_parameters.h"

using namespace std;
using namespace bgen;

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
    cout << "--no-default-includes: disable default include search paths" << endl;
    
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

    code_map symbols = visitor::parse(plugin);
 
    if (error_status::status () != error_status_type::failure)
        plugin->generate (symbols);
}

int main(int arg_c, char * arg_v[]) {
    
	using namespace command_line;

	auto main_expression =
		key("-l", "--language")[&parameters::language] >
		*(
			key ("-i", "--include")[&parameters::include_files] |
            key ("--include-path")[&parameters::include_paths] |
			key ("-s", "--source")[&parameters::source_files] |
			key ("-cp", "--client_path")[&parameters::client_dest] |
			key ("-hd", "--host-path")[&parameters::host_dest] |
			key ("-ca", "--compile-args")[&parameters::compile_args] |
            option ("--no-default-includes")[&parameters::no_default_includes]
		);

	auto expression =
		usage(option("-v", "--version"))[&show_version] |
		usage(option("-h", "--help"))[&show_usage] |
		usage(main_expression)[&process]
	;

	auto & params = parameters::get();
    
    auto parse_result = parse (expression, arg_c, arg_v, params);

	if (!parse_result) {
        error_status::fail ();
        logger::write () << "unexpected command line arguments found";
        
        for (auto & error : parse_result.messages ())
            logger::write () << error;
        
        show_usage(params);
	}
    
    if (error_status::status () == error_status_type::failure)
        return -1;
    
	return 0;
}
