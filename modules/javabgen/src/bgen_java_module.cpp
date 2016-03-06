#include "bgen_java_module.h"

namespace bgen {

	base_language_plugin * make_javabgen_plugin() {
		return new java_plugin();
	}

	string java_plugin::handled_language() const {
		return "java";
	}

	bool java_plugin::handles_language(const std::string & lang) const{
		return lang == "java";
	}

	void java_plugin::generate_host (type_map & types) const {
	}

	void java_plugin::generate_client (type_map & types) const {
	}

}