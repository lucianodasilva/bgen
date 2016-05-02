#include "bgen_casa_generation.h"

#include <fstream>

using namespace std;

namespace bgen {
    namespace casa {
        
        namespace gen {

            js_type cast_type_to_js ( const bgen::type_info::shared & type ) {

                switch (type->kind ()) {
                    case type_kind::type_kind_bool:
                        return js_type::boolean;

                    case type_kind::type_kind_char:
                    case type_kind::type_kind_uchar:
                    case type_kind::type_kind_char16:
                    case type_kind::type_kind_char32:
                    case type_kind::type_kind_ushort:
                    case type_kind::type_kind_uint:
                    case type_kind::type_kind_ulong:
                    case type_kind::type_kind_ulonglong:
                    case type_kind::type_kind_char_s:
                    case type_kind::type_kind_schar:
                    case type_kind::type_kind_wchar:
                    case type_kind::type_kind_short:
                    case type_kind::type_kind_int:
                    case type_kind::type_kind_long:
                    case type_kind::type_kind_longlong:
                    case type_kind::type_kind_int128:
                        return js_type::js_int;

                    case type_kind::type_kind_float:
                    case type_kind::type_kind_double:
                    case type_kind::type_kind_longdouble:
                        return js_type::js_float;

                    case (type_kind::type_kind_struct):
                    {
                        struct_info::shared strt = type->struct_info();

                        string dbg_name = strt->name ();

                        if (!strt)
                            return js_type::unknown;

                        if (
                            strt->name () == "vector" &&
                            strt->namespace_name().size () > 0 &&
                            strt->namespace_name()[0] == "std" &&
                            type->template_params().size () > 0
                        ) {
                            js_type array_js_type = cast_type_to_js(type->template_params()[0].type ());

                            // do not support nested arrays
                            if (array_js_type == js_type::unknown || array_js_type == js_type::array)
                                return js_type::unknown;

                            return js_type::array;
                        }

                        if (
                            strt->name () == "basic_string" &&
                            strt->namespace_name ().size () &&
                            strt->namespace_name ()[0] == "std"
                        )
                            return js_type::string;

                        return js_type::object;
                    }
                    case type_kind::type_kind_lvalue_ref:
                    case type_kind::type_kind_rvalue_ref:
                        return cast_type_to_js(type->base());
                    case type_kind::type_kind_void:
                        return js_type::js_void;
                    case type_kind::type_kind_pointer:
                    case type_kind::type_kind_enum:
                    case type_kind::type_kind_constant_array:
                    case type_kind::type_kind_incomplete_array:
                    case type_kind::type_kind_nullptr:
                    case type_kind::type_kind_invalid:
                    case type_kind::type_kind_unhandled:
                    default:
                        return js_type::unknown;
                }
            }
            
            string namespace_to_string (const namespace_info & info, const string & separator) {
                stringstream stream;
                
                size_t size = info.size ();
                
                if (size > 0)
                    stream << info[0];
                
                for (int i = 1; i < size; ++i)
                    stream << separator << info [i];
                
                return stream.str ();
            }
            
            string namespace_to_uri (const namespace_info & info) {
                return namespace_to_string (info, "/");
            }
            
            string namespace_to_listener (const namespace_info & info) {
                return namespace_to_string (info, "_");
            }
            
            boilerplate::boilerplate ( const string & file )
            : _file_name (file) {}
            
            void boilerplate::write (bgen::gen::output & out) const {
                
                ifstream stream (_file_name);
                
                if (!stream) {
                    logger::write () << "failed to read resource file at " << _file_name;
                    error_status::fail();
                    return;
                }
                
                string line;
                
                while (getline (stream, line))
                    out.line () << line;
            }
        }
    }
}