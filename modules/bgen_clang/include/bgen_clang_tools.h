#pragma once

#ifndef _bgen_clang_tools_h_
#define _bgen_clang_tools_h_

#include <bgen_core.h>
#include <clang-c/Index.h>

namespace bgen {
	namespace clang {
		namespace tools {

			template < class _t >
			inline bgen::source::location get_location(const _t & item, CXSourceLocation (*_location_f)(_t)) {
				auto loc = _location_f(item);
				
				CXFile loc_file;

				uint32_t
					offset,
					line,
					column;
				
				string file;
			
				clang_getSpellingLocation(loc, &loc_file, &line, &column, &offset);

				if (loc_file) {
					auto file_spelling = clang_getFileName(loc_file);
					file = clang_getCString(file_spelling);
					clang_disposeString(file_spelling);
				}

				return { line, column, file };
			}

			inline bgen::source::location get_location(const CXCursor & cursor) {
				return get_location(cursor, clang_getCursorLocation);
			}

			inline bgen::source::location get_location(const CXDiagnostic & diag) {
				return get_location(diag, clang_getDiagnosticLocation);
			}

			template < class _t >
			inline string get_spelling(const _t & item, CXString (*_spell_f)(_t)) {
				auto spell = _spell_f(item);
				auto copy = string(clang_getCString(spell));
				clang_disposeString(spell);
				return copy;
			}

			inline string get_spelling(const CXCursor & cursor) {
				return get_spelling(cursor, clang_getCursorSpelling);
			}

			inline string get_spelling(const CXType & type) {
				return get_spelling(type, clang_getTypeSpelling);
			}

			inline string get_spelling(const CXTypeKind & kind) {
				return get_spelling(kind, clang_getTypeKindSpelling);
			}

			inline string get_spelling(const CXDiagnostic & diag) {
				return get_spelling(diag, clang_getDiagnosticSpelling);
			}
		}

	}
}

#endif