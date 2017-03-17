#include "bgen_parser_struct.h"

namespace bgen {
	namespace parser {

		semantic_name semantic_name_from_path (const parser::semantic_path & path) {
			semantic_name name;

			for (auto & p : path)
				name.push_back(p.identifier);

			return name;
		}

	}
}