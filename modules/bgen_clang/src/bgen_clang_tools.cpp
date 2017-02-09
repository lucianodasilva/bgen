#include "bgen_clang_tools.h"

namespace bgen {
    namespace clang {
        namespace tools {

            bool check_for_errors(const CXTranslationUnit &tu) {
                auto diag_count = clang_getNumDiagnostics(tu);

                for (uint32_t i = 0; i < diag_count; ++i) {
                    auto diag = clang_getDiagnostic(tu, i);

                    auto severity = clang_getDiagnosticSeverity(diag);

                    if (severity == CXDiagnostic_Error || severity == CXDiagnostic_Fatal)
                        return true;
                }

                return false;
            }

            void report_diagnostics(context &cxt, const CXTranslationUnit &tu) {
                auto diag_count = clang_getNumDiagnostics(tu);

                for (uint32_t i = 0; i < diag_count; ++i) {
                    auto diag = clang_getDiagnostic(tu, i);

                    location loc = tools::get_location(diag);
                    string message = tools::get_spelling(diag);

                    cxt.status.warn (loc) << message;
                }
            }

            vector<const char *>
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

            CXTranslationUnit create_translation_unit(
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

        }
    }
}