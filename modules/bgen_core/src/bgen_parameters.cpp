#include "bgen_parameters.h"

namespace bgen {

	parameters & parameters::get() {
		static parameters p;
		return p;
	}

}