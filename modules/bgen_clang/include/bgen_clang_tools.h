#pragma once

#ifndef _bgen_clang_tools_h_
#define _bgen_clang_tools_h_

#include <bgen_core.h>
#include <clang-c/Index.h>

using namespace bgen::source;

namespace bgen {
	namespace clang {
		namespace tools {

			template < class _t >
			inline location get_location(const _t & item, CXSourceLocation (*_location_f)(_t)) {
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

				return { file, line, column };
			}

			inline location get_location(const CXCursor & cursor) {
				return get_location(cursor, clang_getCursorLocation);
			}

			inline location get_location(const CXDiagnostic & diag) {
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

			inline element_type convert_kind (CXCursorKind kind) {
                switch (kind) {
                    case (CXCursor_ClassDecl):
                        return element_type::decl_class;
                    case (CXCursor_StructDecl):
                        return element_type::decl_struct;
                    case (CXCursor_CXXBaseSpecifier):
                        return element_type::decl_base;
                    case (CXCursor_CXXMethod):
                        return element_type::method;
                    case (CXCursor_FunctionDecl):
                        return element_type::function;
                    case (CXCursor_ParmDecl):
                        return element_type::parameter;
                    case (CXCursor_Constructor):
                        return element_type::method;
                    case (CXCursor_FieldDecl):
                        return element_type::field;
                    case (CXCursor_Namespace):
                        return element_type::decl_namespace;
                    default:
                        return element_type::undefined;
                }
            } 

			inline type_kind convert_type_kind (CXTypeKind kind) {
				switch (kind) {
					case (CXType_Invalid) :
						return type_kind::type_kind_invalid;
					case (CXType_Void) :
						return type_kind::type_kind_void;
					case (CXType_Bool) :
						return type_kind::type_kind_bool;
					case (CXType_Char_U) :
						return type_kind::type_kind_char;
					case (CXType_UChar) :
						return type_kind::type_kind_uchar;
					case (CXType_Char16) :
						return type_kind::type_kind_char16;
					case (CXType_Char32) :
						return type_kind::type_kind_char32;
					case (CXType_UShort) :
						return type_kind::type_kind_ushort;
					case (CXType_UInt) :
						return type_kind::type_kind_uint;
					case (CXType_ULong) :
						return type_kind::type_kind_ulong;
					case (CXType_ULongLong) :
						return type_kind::type_kind_ulonglong;
					case (CXType_Char_S) :
						return type_kind::type_kind_char_s;
					case (CXType_SChar) :
						return type_kind::type_kind_schar;
					case (CXType_WChar) :
						return type_kind::type_kind_wchar;
					case (CXType_Short) :
						return type_kind::type_kind_short;
					case (CXType_Int) :
						return type_kind::type_kind_int;
					case (CXType_Long) :
						return type_kind::type_kind_long;
					case (CXType_LongLong) :
						return type_kind::type_kind_longlong;
					case (CXType_Int128) :
						return type_kind::type_kind_int128;
					case (CXType_Float) :
						return type_kind::type_kind_float;
					case (CXType_Double) :
						return type_kind::type_kind_double;
					case (CXType_LongDouble) :
						return type_kind::type_kind_longdouble;
					case (CXType_NullPtr) :
						return type_kind::type_kind_nullptr;
					case (CXType_Pointer) :
						return type_kind::type_kind_pointer;
					case (CXType_LValueReference) :
						return type_kind::type_kind_lvalue_ref;
					case (CXType_RValueReference) :
						return type_kind::type_kind_rvalue_ref;
					case (CXType_Record) :
						return type_kind::type_kind_struct;
					case (CXType_Enum) :
						return type_kind::type_kind_enum;
					case (CXType_ConstantArray) :
						return type_kind::type_kind_constant_array;
					case (CXType_IncompleteArray) :
						return type_kind::type_kind_incomplete_array;
					default:
						return type_kind::type_kind_unhandled;
				}
			}
		}
	}
}

#endif