#include "bgen_clang_parser.h"
#include "bgen_clang_tools.h"
#include "bgen_clang_visitor.h"

#include <list>
#include <vector>

using namespace std;
using namespace bgen::source;

namespace bgen {
    namespace clang {

        const string pre_compiled_headers = "bgen.pch";

        parser_result parser::parse ( bgen::context & cxt ) {
            parser_result result {};

            auto & params = cxt.parameters;

            auto pch_file = bgen::system::merge_path (params.client_dest, pre_compiled_headers);
            auto include_search_paths = params.include_paths;

            // check for default include search paths
            if (!params.no_default_includes) {
                auto default_includes = bgen::system::get_include_paths ();

                include_search_paths.reserve (
                    include_search_paths.size () + default_includes.size ()
                );

                include_search_paths.insert (
                    include_search_paths.end (),
                    default_includes.begin (),
                    default_includes.end ()
                );
            }

            // build clang argument list
            auto args = tools::make_clang_arguments(params.include_files, include_search_paths);

            auto index = clang_createIndex(0, 0);
            auto_guard([&index]() {
                clang_disposeIndex(index);
            });

            bool is_first_file = true;
            // for each source file
            for (auto & source : params.source_files) {

                CXTranslationUnit tu = tools::create_translation_unit (
                    index,
                    source,
                    args,
                    is_first_file
                );

                if (is_first_file) {
                    args.push_back("-include-pch");
                    args.push_back(pch_file.c_str ());
                    is_first_file = false;
                }

                auto_guard([&tu]() {
                    clang_disposeTranslationUnit(tu);
                });

                auto diag_count = clang_getNumDiagnostics(tu);

                // check for diagnostics
                if (diag_count) {
                    //TODO: diagnostic messages outside the set
                    //		of source files should not be considered
                    tools::report_diagnostics(cxt, tu);

                    if (tools::check_for_errors (tu)) {
                        tools::report_diagnostics (cxt, tu);
                        return {};
                    }
                }

                // list types in source file
                auto cursor = clang_getTranslationUnitCursor(tu);

                visitor::visitor_context visitor_cxt = { cxt, result };

                clang_visitChildren(cursor, visitor::visit_child, &visitor_cxt);

                // clean up
                clang_saveTranslationUnit(tu, pch_file.c_str(), clang_defaultSaveOptions(tu));
            }

            return result;
        }
    }
}