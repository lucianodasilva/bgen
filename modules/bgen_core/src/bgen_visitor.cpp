#include <clang-c/Index.h>
#include <list>
#include "bgen_visitor.h"

#include "bgen_clang.h"
#include "bgen_details.h"
#include "bgen_logger.h"
#include "bgen_parameters.h"
#include "bgen_error_status.h"
#include "bgen_system.h"

namespace bgen {

	struct visitor::internal {
        
        static inline bool check_for_errors (const CXTranslationUnit & tu ) {
            auto diag_count = clang_getNumDiagnostics (tu);
            
            for (uint32_t i = 0; i < diag_count; ++i) {
                auto diag = clang_getDiagnostic(tu, i);
                
                auto severity = clang_getDiagnosticSeverity(diag);
                
                if (severity == CXDiagnostic_Error || severity == CXDiagnostic_Fatal)
                    return true;
            }
            
            return false;
        }

		/* tools */
		static inline void report_diagnostics(context & cxt, const CXTranslationUnit & tu) {
			auto diag_count = clang_getNumDiagnostics(tu);

			for (uint32_t i = 0; i < diag_count; ++i) {
				auto diag = clang_getDiagnostic(tu, i);

				source_location loc = clang::get_location(diag);
				string message = clang::get_spelling(diag);

                logger::write(loc) << message;
			}
		}

		static inline vector < const char * > make_clang_arguments (const vector < string > & definitions, const vector < string > & includes ) {
			vector < const char * > arguments = {
				"-fsyntax-only", "-x", "c++", "-std=c++11" // defaults ( set clang to not compile only build ast )
			};

			for (auto & d : definitions) {
				arguments.push_back("-D");
				arguments.push_back(d.c_str());
			}

			for (auto & i : includes) {
				arguments.push_back("-I");
				arguments.push_back(i.c_str());
			}

			return arguments;
		}

		static inline CXTranslationUnit create_translation_unit(
			const CXIndex & index,
			const string & source,
			vector < const char * > arguments,
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
					CXTranslationUnit_ForSerialization | CXTranslationUnit_SkipFunctionBodies
				);
			}
			else {
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
		static CXChildVisitResult handle_visit_children(
			CXCursor cursor,
			CXCursor parent,
			CXClientData client_data
		);

		static inline void handle_inplace_struct (context & cxt, const CXType & src_type, type_info * type) {
			auto decl_cursor = clang_getTypeDeclaration (src_type);
			auto decl_type = clang_getCursorType(decl_cursor);

			string native_name = clang::get_spelling(decl_type);
			string stct_name = clang::get_spelling (decl_cursor);

			bool is_struct = decl_cursor.kind == CXCursor_StructDecl || decl_cursor.kind == CXCursor_ClassDecl;

			if (!is_struct)
				return;

			type->kind = type_kind::type_kind_struct;
			type->base_struct = get_or_make_struct (cxt.symbols, native_name);

			type->base_struct->name = stct_name;

			// extract namespace
			list < string > nspace_info;
			CXCursor parent = clang_getCursorSemanticParent(decl_cursor);

			while (parent.kind == CXCursorKind::CXCursor_Namespace) {
				nspace_info.push_front (clang::get_spelling(parent));
				parent = clang_getCursorSemanticParent(parent);
			}

			type->base_struct->namespace_name = namespace_info (nspace_info.begin (), nspace_info.end ());

			int t_num = clang_Type_getNumTemplateArguments(src_type);
			if (t_num != 0) {
				for (int ti = 0; ti < t_num; ++ti) {
					auto ttype = clang_Type_getTemplateArgumentAsType(src_type, ti);
					if (ttype.kind == CXType_Invalid) {
						logger::write (clang::get_location (decl_cursor)) << "unsupported template argument kind";
						continue;
					}
                    
                    type->template_params.push_back (template_param_info {handle_type(cxt, ttype)});
				}
			}
		}

		static inline type_kind handle_type_kind(context & cxt, const CXType & type) {
			auto kind = type.kind;

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

		static inline bool is_valid_kind(const type_info * info) {
			return info->kind != type_kind::type_kind_invalid && info->kind != type_kind::type_kind_unhandled;
		}
        
        static inline void register_dependency ( context & cxt, type_info * type ) {
            
            if (!cxt.active_struct)
                return;
                
            auto root = find_root_type(type);
            
            // if is not a structure cancel
            if (root->kind != type_kind::type_kind_struct)
                return;
                
            bool is_soft_dependency = 
                type->kind == type_kind::type_kind_pointer ||
                type->kind == type_kind::type_kind_lvalue_ref ||
                type->kind == type_kind::type_kind_rvalue_ref;
            
            // register dependency
            const string & native_name = root->base_struct->native_name;
            
            auto it = cxt.active_dependencies.find (native_name);
                
            if (it != cxt.active_dependencies.end ())
                return;
    
            cxt.active_dependencies [native_name] = {
                native_name ,
                is_soft_dependency
            };
        }

		static inline type_info * handle_type(context & cxt, const CXType & native_type) {
            
            CXType src_type = native_type;
            
            while (src_type.kind == CXType_Typedef)
                src_type = clang_getCanonicalType(src_type);
			
			string native_name = clang::get_spelling(src_type);

            bool is_new_type = cxt.symbols.types.find (native_name) == cxt.symbols.types.end ();
			auto type = get_or_make_type (cxt.symbols, native_name);
            
			if (is_new_type) {

				type->dimention = 0;
				type->is_const = clang_isConstQualifiedType(src_type) != 0;
				type->kind = handle_type_kind(cxt, src_type);
				type->native_name = native_name;

				switch (type->kind) {
				case (type_kind::type_kind_pointer) :
				case (type_kind::type_kind_lvalue_ref) :
				case (type_kind::type_kind_rvalue_ref) :
					type->base_type = handle_type(cxt, clang_getPointeeType(src_type));
					break;
				case (type_kind::type_kind_struct) :
					handle_inplace_struct(cxt, src_type, type);
					break;
                case (type_kind::type_kind_unhandled) :
					{
						int t_num = clang_Type_getNumTemplateArguments(src_type);
						if (t_num != 0)
							handle_inplace_struct(cxt, src_type, type);
					}
                    break;
				case (type_kind::type_kind_enum) :
					// still not supported
					type->kind = type_kind::type_kind_unhandled;
					break;
				case (type_kind::type_kind_constant_array) :
					type->dimention = static_cast <uint32_t> (clang_getNumElements(src_type));
					type->base_type = handle_type(cxt, clang_getArrayElementType(src_type));
					break;
				case (type_kind::type_kind_incomplete_array) :
					type->base_type = handle_type(cxt, clang_getArrayElementType(src_type));
					break;
				default:
					break;
				}

			}
            
            register_dependency ( cxt, type );

			if (type->base_type && !is_valid_kind(type->base_type)) {
				// invalid or unhandled base type should 
				// render children invalid / unhandled
				type->kind = type->base_type->kind;
                type->base_type = nullptr;
			}

			return type;
		}

		static inline void handle_struct_def(context & cxt, const CXCursor & cursor, struct_type type) {

			string name = clang::get_spelling(cursor);

			CXType clang_type = clang_getCursorType(cursor);
			string clang_type_name = clang::get_spelling(clang_type);

			// backup active struct and dependencies
			auto backup_struct = cxt.active_struct;
            
            // create empty and swap
            auto backup_dependencies = map < string, dependency > ();
            swap (backup_dependencies, cxt.active_dependencies);

			// create and fill struct info
			cxt.active_struct = get_or_make_struct (cxt.symbols, clang_type_name);
            cxt.active_struct->is_visited = true;
			cxt.active_struct->name = name;
			cxt.active_struct->native_name = clang_type_name;
			cxt.active_struct->namespace_name = cxt.active_namespace;
			cxt.active_struct->type = type;
            cxt.active_struct->location = clang::get_location (cursor);
            
			// visit children
			clang_visitChildren(cursor, handle_visit_children, &cxt);

            // pack dependency map into struct
            for ( auto d : cxt.active_dependencies ) {
                cxt.active_struct->dependencies.push_back (d.second);
            }

			cxt.active_struct = backup_struct;
            swap (backup_dependencies, cxt.active_dependencies);
		}

		static inline void handle_struct_base(context & cxt, const CXCursor & cursor) {
			CX_CXXAccessSpecifier access = clang_getCXXAccessSpecifier(cursor);
			
			visibility_type vis = visibility_type::visibility_private;

			if (access == CX_CXXPublic)
				vis = visibility_type::visibility_public;

			auto native_type = clang_getCursorType(cursor);
			auto native_name = clang::get_spelling(native_type);
            
            cxt.active_struct->base_structs.push_back (get_or_make_struct (cxt.symbols, native_name));
		}

		static inline void handle_method_def(context & cxt, const CXCursor & cursor, bool is_constructor) {

            bool is_global = cxt.active_struct == cxt.symbols.global.get ();
            
            visibility_type vis = visibility_type::visibility_private;
            
            if (is_global) {
                vis = visibility_type::visibility_public;
            } else {
                CX_CXXAccessSpecifier access = clang_getCXXAccessSpecifier(cursor);

                if (access == CX_CXXProtected)
                    vis = visibility_type::visibility_protected;

                if (access == CX_CXXPublic)
                    vis = visibility_type::visibility_public;
            }

            cxt.active_method = {};
            
            cxt.active_method.name = clang::get_spelling(cursor);
            cxt.active_method.namespace_name = cxt.active_namespace;
            cxt.active_method.location = clang::get_location (cursor);
            cxt.active_method.visibility = vis;
            cxt.active_method.is_virtual = clang_CXXMethod_isVirtual(cursor) != 0;
            cxt.active_method.is_pure = clang_CXXMethod_isPureVirtual(cursor) != 0;
            cxt.active_method.is_static = clang_CXXMethod_isStatic(cursor) != 0 || is_global;
            cxt.active_method.is_const = clang_CXXMethod_isConst(cursor) != 0;
            cxt.active_method.is_ctor = is_constructor;

			if (!is_constructor) {
				// unfold return type
				auto ftype = clang_getCursorType(cursor);
				auto rtype = clang_getResultType(ftype);

				cxt.active_method.return_type = handle_type(cxt, rtype);

				clang_visitChildren(cursor, handle_visit_children, &cxt);

				if (!is_valid_kind(cxt.active_method.return_type)) {
                    logger::write (clang::get_location(cursor)) << "invalid or unexpected type for method";
					return;
				}
			}

			// add namespace for use with globals
			cxt.active_method.namespace_name = cxt.active_namespace;

			// TODO: check for validity of parameters
			cxt.active_struct->methods.push_back(cxt.active_method);
		}

		static inline void handle_param_def(context & cxt, const CXCursor & cursor) {
			auto native_type = clang_getCursorType(cursor);
			auto type = handle_type(cxt, native_type);
            
            cxt.active_method.params.emplace_back();
            auto & p = cxt.active_method.params.back ();
            
            p.name = clang::get_spelling (cursor);
            p.type = type;
            
		}

		static inline void handle_field_def(context & cxt, const CXCursor & cursor) {
			CX_CXXAccessSpecifier access = clang_getCXXAccessSpecifier(cursor);

			visibility_type vis = visibility_type::visibility_private;

			if (access == CX_CXXProtected)
				vis = visibility_type::visibility_protected;

			if (access == CX_CXXPublic)
				vis = visibility_type::visibility_public;

			auto name = clang::get_spelling(cursor);
			auto native_type = clang_getCursorType(cursor);

			auto type = handle_type(cxt, native_type);

			if (!is_valid_kind(type)) {
                logger::write(clang::get_location(cursor)) << "invalid or unexpected type for field";
				return;
			}

            cxt.active_struct->fields.emplace_back ();
            
            auto & f = cxt.active_struct->fields.back ();
            
            f.namespace_name = cxt.active_namespace;
            f.name = name;
            f.type = type;
            f.location = clang::get_location (cursor);
            f.visibility = vis;

		}

		static inline void handle_namespace_def(context & cxt, const CXCursor & cursor) {
			cxt.active_namespace.push_back(clang::get_spelling(cursor));

			// visit children
			clang_visitChildren(cursor, handle_visit_children, &cxt);

			// pop back to previous state
			cxt.active_namespace.pop_back();
		}

	};

	CXChildVisitResult visitor::internal::handle_visit_children(
		CXCursor cursor,
		CXCursor parent,
		CXClientData client_data
	) {
        
        if (clang_Location_isFromMainFile(clang_getCursorLocation(cursor)) == 0)
            return CXChildVisit_Continue;

		auto & cxt = *(context *)client_data;
		auto kind = clang_getCursorKind(cursor);

		switch (kind) {
		case CXCursorKind::CXCursor_ClassDecl:
			internal::handle_struct_def(cxt, cursor, struct_type::struct_type_class);
			break;
		case CXCursorKind::CXCursor_StructDecl:
			internal::handle_struct_def(cxt, cursor, struct_type::struct_type_struct);
			break;
		case CXCursorKind::CXCursor_CXXBaseSpecifier:
			internal::handle_struct_base(cxt, cursor);
			break;
		case CXCursorKind::CXCursor_CXXMethod:
			if (cxt.active_struct)
                internal::handle_method_def(cxt, cursor, false);
			break;
        case CXCursorKind::CXCursor_FunctionDecl:
            if (!cxt.active_struct) {
                cxt.active_struct = cxt.symbols.global.get ();
                internal::handle_method_def(cxt, cursor, false);
                cxt.active_struct = nullptr;
            } else {
                internal::handle_method_def (cxt, cursor, false);
            }
            break;
		case CXCursorKind::CXCursor_ParmDecl:
			internal::handle_param_def(cxt, cursor);
			break;
		case CXCursorKind::CXCursor_FunctionTemplate:
            logger::write (clang::get_location(cursor)) << "function templates not supported";
            error_status::warn();
			break;
		case CXCursorKind::CXCursor_Constructor:
			internal::handle_method_def (cxt, cursor, true);
			break;
		case CXCursorKind::CXCursor_Destructor:
			break;
		case CXCursorKind::CXCursor_FieldDecl:
			handle_field_def(cxt, cursor);
			break;
		case CXCursorKind::CXCursor_EnumDecl:
			break;
		case CXCursorKind::CXCursor_Namespace:
			internal::handle_namespace_def(cxt, cursor);
			break;
		default:
			break;
		}

		return CXChildVisitResult::CXChildVisit_Continue;
	}

	code_map visitor::parse() {
		context cxt;

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
			clang_visitChildren(cursor, internal::handle_visit_children, &cxt);

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
