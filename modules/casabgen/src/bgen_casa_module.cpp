#include "bgen_casa_module.h"
#include "bgen_casa_common.h"
#include "bgen_casa_generation.h"
#include "bgen_casa_validation.h"
#include "bgen_casa_cpp_generation.h"
#include "bgen_casa_js_generation.h"

#include <algorithm>

using namespace std;

namespace bgen {

    base_language_plugin * make_casabgen_plugin() {
        return new casablanca_plugin();
    }

    string casablanca_plugin::handled_language() const {
        return "casablanca";
    }

    bool casablanca_plugin::handles_language(const std::string & lang) const{
        return lang == "casablanca";
    }

    void casablanca_plugin::generate (bgen::type_map & types) const {

        casa::type_map intermediate;

        casa::validation::validate_and_set(types, intermediate);

        bgen::casa::gen::cpp::generate (
            types,
            "casa_rest_server.h",
            "resources/casablanca/parser_boilerplate.txt",
            "resources/casablanca/listener_boilerplate.txt"
        );

        bgen::casa::gen::js::generate (
            intermediate,
            "casa_rest_client.js",
            "resources/casablanca/js_boilerplate.txt"
        );

    }


}