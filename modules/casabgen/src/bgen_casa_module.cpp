#include "bgen_casa_module.h"
#include "bgen_casa_common.h"
#include "bgen_casa_generation.h"
#include "bgen_casa_validation.h"
#include "bgen_casa_cpp_generation.h"
#include "bgen_casa_js_generation.h"

#include <algorithm>

using namespace std;

namespace bgen {

    base_plugin * make_casabgen_plugin() {
        return new casablanca_plugin();
    }

    string casablanca_plugin::public_name () const {
        return "casablanca";
    }

    bool casablanca_plugin::accepts (const std::string & lang) const{
        return lang == "casablanca";
    }

    void casablanca_plugin::generate (const code_map & symbols) const {

        casa::type_map intermediate;

        casa::validation::validate_and_set(symbols, intermediate);

        bgen::casa::gen::cpp::generate (
            intermediate,
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