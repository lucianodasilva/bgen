// //
// // Created by Luciano Silva on 15/12/16.
// //
// #include "bgen_clang_type_maker.h"

// using namespace std;

// namespace bgen{
//     namespace clang {

//         static inline void register_dependency ( visitor_context & cxt, type_info * type ) {

//             if (!cxt.active_struct)
//                 return;

//             auto root = find_root_type(type);

//             // if is not a structure cancel
//             if (root->kind != type_kind::type_kind_struct)
//                 return;

//             bool is_soft_dependency =
//                     type->kind == type_kind::type_kind_pointer ||
//                     type->kind == type_kind::type_kind_lvalue_ref ||
//                     type->kind == type_kind::type_kind_rvalue_ref;

//             // register dependency
//             const string & native_name = root->base_struct->native_name;

//             auto it = cxt.active_dependencies.find (native_name);

//             if (it != cxt.active_dependencies.end ())
//                 return;

//             cxt.active_dependencies [native_name] = {
//                     native_name ,
//                     is_soft_dependency
//             };
//         }

//         static inline type_kind handle_type_kind(visitor_context & cxt, const CXType & type) {
//             auto kind = type.kind;

//             switch (kind) {
//                 case (CXType_Invalid) :
//                     return type_kind::type_kind_invalid;
//                 case (CXType_Void) :
//                     return type_kind::type_kind_void;
//                 case (CXType_Bool) :
//                     return type_kind::type_kind_bool;
//                 case (CXType_Char_U) :
//                     return type_kind::type_kind_char;
//                 case (CXType_UChar) :
//                     return type_kind::type_kind_uchar;
//                 case (CXType_Char16) :
//                     return type_kind::type_kind_char16;
//                 case (CXType_Char32) :
//                     return type_kind::type_kind_char32;
//                 case (CXType_UShort) :
//                     return type_kind::type_kind_ushort;
//                 case (CXType_UInt) :
//                     return type_kind::type_kind_uint;
//                 case (CXType_ULong) :
//                     return type_kind::type_kind_ulong;
//                 case (CXType_ULongLong) :
//                     return type_kind::type_kind_ulonglong;
//                 case (CXType_Char_S) :
//                     return type_kind::type_kind_char_s;
//                 case (CXType_SChar) :
//                     return type_kind::type_kind_schar;
//                 case (CXType_WChar) :
//                     return type_kind::type_kind_wchar;
//                 case (CXType_Short) :
//                     return type_kind::type_kind_short;
//                 case (CXType_Int) :
//                     return type_kind::type_kind_int;
//                 case (CXType_Long) :
//                     return type_kind::type_kind_long;
//                 case (CXType_LongLong) :
//                     return type_kind::type_kind_longlong;
//                 case (CXType_Int128) :
//                     return type_kind::type_kind_int128;
//                 case (CXType_Float) :
//                     return type_kind::type_kind_float;
//                 case (CXType_Double) :
//                     return type_kind::type_kind_double;
//                 case (CXType_LongDouble) :
//                     return type_kind::type_kind_longdouble;
//                 case (CXType_NullPtr) :
//                     return type_kind::type_kind_nullptr;
//                 case (CXType_Pointer) :
//                     return type_kind::type_kind_pointer;
//                 case (CXType_LValueReference) :
//                     return type_kind::type_kind_lvalue_ref;
//                 case (CXType_RValueReference) :
//                     return type_kind::type_kind_rvalue_ref;
//                 case (CXType_Record) :
//                     return type_kind::type_kind_struct;
//                 case (CXType_Enum) :
//                     return type_kind::type_kind_enum;
//                 case (CXType_ConstantArray) :
//                     return type_kind::type_kind_constant_array;
//                 case (CXType_IncompleteArray) :
//                     return type_kind::type_kind_incomplete_array;
//                 default:
//                     return type_kind::type_kind_unhandled;
//             }

//         }

//         inline void handle_inplace_struct (visitor_context & cxt, const CXType & src_type, type_info * type) {
//             auto decl_cursor = clang_getTypeDeclaration (src_type);
//             auto decl_type = clang_getCursorType(decl_cursor);

//             string native_name = clang::get_spelling(decl_type);
//             string stct_name = clang::get_spelling (decl_cursor);

//             bool is_struct = decl_cursor.kind == CXCursor_StructDecl || decl_cursor.kind == CXCursor_ClassDecl;

//             if (!is_struct)
//                 return;

//             type->kind = type_kind::type_kind_struct;
//             type->base_struct = get_or_make_struct (cxt.symbols, native_name);

//             type->base_struct->name = stct_name;

//             // extract namespace
//             list < string > nspace_info;
//             CXCursor parent = clang_getCursorSemanticParent(decl_cursor);

//             while (parent.kind == CXCursorKind::CXCursor_Namespace) {
//                 nspace_info.push_front (clang::get_spelling(parent));
//                 parent = clang_getCursorSemanticParent(parent);
//             }

//             type->base_struct->namespace_name = {nspace_info.begin (), nspace_info.end ()};

//             int t_num = clang_Type_getNumTemplateArguments(src_type);
//             if (t_num != 0) {
//                 for (int ti = 0; ti < t_num; ++ti) {
//                     auto ttype = clang_Type_getTemplateArgumentAsType(src_type, ti);
//                     if (ttype.kind == CXType_Invalid) {
//                         //logger::write (clang::get_location (decl_cursor)) << "unsupported template argument kind";
//                         continue;
//                     }

//                     type->template_params.push_back (template_param_info {make_type(cxt, ttype)});
//                 }
//             }
//         }

//         type_info * make_type (visitor_context & cxt, const CXType & native_type) {

//             CXType src_type = native_type;

//             while (src_type.kind == CXType_Typedef)
//                 src_type = clang_getCanonicalType(src_type);

//             string native_name = clang::get_spelling(src_type);

//             bool is_new_type = cxt.symbols.types.find (native_name) == cxt.symbols.types.end ();
//             auto type = get_or_make_type (cxt.symbols, native_name);

//             if (is_new_type) {

//                 type->dimention = 0;
//                 type->is_const = clang_isConstQualifiedType(src_type) != 0;
//                 type->kind = handle_type_kind(cxt, src_type);
//                 type->native_name = native_name;

//                 switch (type->kind) {
//                     case (type_kind::type_kind_pointer) :
//                     case (type_kind::type_kind_lvalue_ref) :
//                     case (type_kind::type_kind_rvalue_ref) :
//                         type->base_type = make_type (cxt, clang_getPointeeType(src_type));
//                         break;
//                     case (type_kind::type_kind_struct) :
//                         handle_inplace_struct(cxt, src_type, type);
//                         break;
//                     case (type_kind::type_kind_unhandled) : {
//                         int t_num = clang_Type_getNumTemplateArguments(src_type);
//                         if (t_num != 0)
//                             handle_inplace_struct(cxt, src_type, type);
//                     }
//                         break;
//                     case (type_kind::type_kind_enum) :
//                         // still not supported
//                         type->kind = type_kind::type_kind_unhandled;
//                         break;
//                     case (type_kind::type_kind_constant_array) :
//                         type->dimention = static_cast <uint32_t> (clang_getNumElements(src_type));
//                         type->base_type = make_type (cxt, clang_getArrayElementType(src_type));
//                         break;
//                     case (type_kind::type_kind_incomplete_array) :
//                         type->base_type = make_type (cxt, clang_getArrayElementType(src_type));
//                         break;
//                     default:
//                         break;
//                 }
//             }

//             return type;
//         }

//     }
// }