#include "bgen_clang_driver.h"
#include "bgen_clang_tools.h"

#include <list>
#include <string>

using namespace std;

namespace bgen {
    namespace clang {

        const element_vector & driver::elements () const {
            return _elements;
        }

        type driver::get_type (const element & e) const {
            return {};
        }

        type driver::get_type (const type & t) const {
            return {};
        }

        template_parameter_vector driver::get_template_parameters (const element & e) const {
            return {};
        }

        template_parameter_vector driver::get_template_parameters (const type & t) const {
            return {};
        }

        semantic_path driver::get_semantic_path (const element & e) const {
            return {};
        }

        semantic_path driver::get_semantic_path (const type & t) const {
            list < string > tmp_parent_names;
            CXCursor parent = clang_getCursorSemanticParent(decl_cursor);

            while (parent.kind == CXCursorKind::CXCursor_Namespace) {
                nspace_info.push_front (tools::get_spelling(parent));
                parent = clang_getCursorSemanticParent(parent);
            }

            visitor_cxt.result.structs [type.base_struct_id].namespace_name = {nspace_info.begin (), nspace_info.end ()};

        }

        driver * driver::make_driver (context & cxt) {
            return nullptr;
        }

    }
}