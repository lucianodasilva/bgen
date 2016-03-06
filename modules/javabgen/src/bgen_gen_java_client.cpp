#include "bgen_gen_java_client.h"

using namespace std;

namespace bgen {
    namespace gen {

        void java_helper::write_visibility(
                visibility_type visibility,
                ostream & stream
        ) {
            switch (visibility) {
                case (visibility_type::visibility_private):
                    stream << "public ";
                    break;
                case (visibility_type::visibility_protected):
                    stream << "protected ";
                    break;
                case (visibility_type::visibility_public):
                    stream << "public ";
                    break;
                default:
                    // TODO: perhaps not the best choice for invalid cases
                    stream << "private ";
                    break;
            }
        }

        void java_helper::write_package(
                const namespace_info &package,
                ostream &stream
        ) {
            if (package.size () == 0)
                return;

            auto it = package.begin ();
            stream << *it;

            for (size_t i = 1; i < package.size (); ++i)
                stream << '.' << package [i];

            stream << ' ';
        }

        bool java_helper::write_basic_type(
                type_kind kind,
                ostream &stream
        ) {
            switch (kind) {
                case type_kind::type_kind_void:
                    stream << "void ";
                    break;
                case type_kind::type_kind_bool:
                    stream << "boolean ";
                    break;
                case type_kind::type_kind_char:
                    stream << "byte ";
                    break;
                case type_kind::type_kind_uchar:
                    stream << "char ";
                    break;
                case type_kind::type_kind_char16:
                    stream << "char ";
                    break;
                case type_kind::type_kind_char32:
                    stream << "char ";
                    break;
                case type_kind::type_kind_ushort:
                    stream << "int ";
                    break;
                case type_kind::type_kind_uint:
                    stream << "long ";
                    break;
                case type_kind::type_kind_ulong:
                    stream << "long ";
                    break;
                case type_kind::type_kind_ulonglong:
                    stream << "long ";
                    break;
                case type_kind::type_kind_char_s:
                    stream << "char ";
                    break;
                case type_kind::type_kind_schar:
                    stream << "char ";
                    break;
                case type_kind::type_kind_wchar:
                    stream << "char ";
                    break;
                case type_kind::type_kind_short:
                    stream << "short ";
                    break;
                case type_kind::type_kind_int:
                    stream << "int ";
                    break;
                case type_kind::type_kind_long:
                    stream << "long ";
                    break;
                case type_kind::type_kind_longlong:
                    stream << "long ";
                    break;
                case type_kind::type_kind_int128:
                    stream << "long ";
                    break;
                case type_kind::type_kind_float:
                    stream << "float ";
                    break;
                case type_kind::type_kind_double:
                    stream << "double ";
                    break;
                case type_kind::type_kind_longdouble:
                    stream << "double ";
                    break;
                case (type_kind::type_kind_struct):
                case type_kind::type_kind_pointer:break;
                case type_kind::type_kind_lvalue_ref:break;
                case type_kind::type_kind_rvalue_ref:break;
                case type_kind::type_kind_enum:break;
                case type_kind::type_kind_constant_array:break;
                case type_kind::type_kind_incomplete_array:break;

                case type_kind::type_kind_nullptr:
                case type_kind::type_kind_invalid:
                case type_kind::type_kind_unhandled:
                default:
                    // TODO: Add "unexpected type" kind of report.
                    return false;
                    break;
            }

            return true;
        }

        bool java_helper::write_array(
                type_info::shared type,
                const string &identifier,
                ostream &stream
        ) {
            // check for proper dimentions
            int dimentions = 0;

            while (
                    type->kind () == type_kind::type_kind_constant_array ||
                    type->kind () == type_kind::type_kind_incomplete_array
            ) {
                ++dimentions;
                type = type->base ();
            }

            if (!write_type (type, "", stream))
                return false;

            if (!identifier.empty())
                stream << identifier << ' ';

            while (dimentions) {
                stream << "[]";
                --dimentions;
            }

            return true;
        }

        bool java_helper::write_type(
                const type_info::shared &type,
                const string & identifier,
                ostream &stream
        ) {
            switch (type->kind ()) {
                case (type_kind::type_kind_struct):
                {
                    auto strct = type->struct_info();

                    write_package (strct->namespace_name (), stream);
                    stream << strct->name () << ' ';
                }
                    break;
                case type_kind::type_kind_constant_array:
                case type_kind::type_kind_incomplete_array:
                    // should be the same case in any usage
                    write_array (type, identifier, stream);
                    break;

                case type_kind::type_kind_enum:
                    // not implemented yet
                case type_kind::type_kind_lvalue_ref:
                case type_kind::type_kind_rvalue_ref:
                case type_kind::type_kind_pointer:
                    // references have no meaning
                    write_type (type->base (), identifier, stream);
                    break;
                case type_kind::type_kind_nullptr:
                case type_kind::type_kind_invalid:
                case type_kind::type_kind_unhandled:
                    // TODO: Add "unexpected type" kind of report.
                    return false;
                default:
                    write_basic_type(type->kind(), stream);
            }

            return true;
        }

        // package
        java_package::java_package(const namespace_info & namespace_name)
                : _namespace (namespace_name)
        {}

        void java_package::write(ostream &stream) const {
            if (_namespace.empty())
                return;

            stream << "package ";

            java_helper::write_package (_namespace, stream);

            stream << ';' << endl;
        }

        // field
        java_field::java_field(const field_info &field_v)
                : _field (field_v)
        {}

        void java_field::write(ostream &stream) const {
            // write visibility
            java_helper::write_visibility(_field.visibility, stream);
            // write type
            java_helper::write_type (_field.type, _field.name, stream);
            // end of line
            stream << ';' << endl;
        }
    }
}