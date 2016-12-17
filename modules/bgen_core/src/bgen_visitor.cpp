#include <clang-c/Index.h>
#include <list>
#include <bgen_clang_handlers.h>
#include "bgen_visitor.h"

#include "bgen_clang.h"
#include "bgen_details.h"
#include "bgen_logger.h"
#include "bgen_parameters.h"
#include "bgen_error_status.h"
#include "bgen_system.h"

namespace bgen {

	struct visitor::internal {

		static inline bool check_for_errors(const CXTranslationUnit &tu) {
			auto diag_count = clang_getNumDiagnostics(tu);

			for (uint32_t i = 0; i < diag_count; ++i) {
				auto diag = clang_getDiagnostic(tu, i);

				auto severity = clang_getDiagnosticSeverity(diag);

				if (severity == CXDiagnostic_Error || severity == CXDiagnostic_Fatal)
					return true;
			}

			return false;
		}

		/* tools */
		static inline void report_diagnostics(visitor_context &cxt, const CXTranslationUnit &tu) {
			auto diag_count = clang_getNumDiagnostics(tu);

			for (uint32_t i = 0; i < diag_count; ++i) {
				auto diag = clang_getDiagnostic(tu, i);

				source_location loc = clang::get_location(diag);
				string message = clang::get_spelling(diag);

				logger::write(loc) << message;
			}
		}

		static inline vector<const char *>
		make_clang_arguments(const vector<string> &definitions, const vector<string> &includes) {
			vector<const char *> arguments = {
					"-fsyntax-only", "-x", "c++", "-std=c++14" // defaults ( set clang to not compile only build ast )
			};

			for (auto &d : definitions) {
				arguments.push_back("-D");
				arguments.push_back(d.c_str());
			}

			for (auto &i : includes) {
				arguments.push_back("-I");
				arguments.push_back(i.c_str());
			}

			return arguments;
		}

		static inline CXTranslationUnit create_translation_unit(
				const CXIndex &index,
				const string &source,
				vector<const char *> arguments,
				bool is_first
		) {
			if (is_first) {
				return clang_parseTranslationUnit(
						index,
						source.c_str(),
						arguments.data(),
						static_cast < int > (arguments.size()),
						nullptr,
						0,
						CXTranslationUnit_ForSerialization |
						CXTranslationUnit_SkipFunctionBodies |
						CXTranslationUnit_DetailedPreprocessingRecord
				);
			} else {
				return clang_createTranslationUnitFromSourceFile(
						index,
						source.c_str(),
						static_cast < int > (arguments.size()),
						arguments.data(),
						0,
						nullptr
				);
			}
		}

		/* cursor conversion */
		static CXChildVisitResult visit_child(
				CXCursor cursor,
				CXCursor parent,
				CXClientData client_data
		);

	};

	CXChildVisitResult visitor::internal::visit_child(
			CXCursor cursor,
			CXCursor parent,
			CXClientData client_data
	) {
        if (clang_Location_isFromMainFile(clang_getCursorLocation(cursor)) == 0)
            return CXChildVisit_Continue;

		auto & cxt = *(visitor_context *)client_data;
		auto kind = clang_getCursorKind(cursor);

		auto handler = clang::handlers::lookup::get(kind);

		handler.visit_start(cxt, cursor);

		clang_visitChildren(cursor, &visit_child, client_data);

		handler.visit_end (cxt, cursor);

		return CXChildVisitResult::CXChildVisit_Continue;
	}

	code_map visitor::parse() {
		visitor_context cxt;

		auto & params = parameters::get();

		string pch_file = params.client_dest + "/bgen.pch";

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

		auto args = internal::make_clang_arguments(params.include_files, include_search_paths);

		auto index = clang_createIndex(0, 0);
		auto_guard([&index]() {
			clang_disposeIndex(index);
		});

		bool is_first_file = true;
		// for each source file
		for (auto & source : params.source_files) {

			cxt.active_source_file = source;

			CXTranslationUnit tu = internal::create_translation_unit(
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
				internal::report_diagnostics(cxt, tu);
                error_status::warn ();

                if (internal::check_for_errors (tu)) {
                    error_status::fail ();
                    return {};
                }
			}

			// list types in source file
			auto cursor = clang_getTranslationUnitCursor(tu);
			clang_visitChildren(cursor, internal::visit_child, &cxt);

			// clean up
			clang_saveTranslationUnit(tu, pch_file.c_str(), clang_defaultSaveOptions(tu));
		}

		// pack structures and sort per dependencies
		for (auto & s : cxt.symbols.structs) {
			cxt.symbols.sorted_dependencies.push_back (s.second.get ());
		}

        cxt.symbols.sorted_dependencies = arrange_by_dependencies(cxt.symbols.sorted_dependencies);

        if (error_status::status () == error_status_type::failure)
            return {};
        else
            return move (cxt.symbols);
	}

}
