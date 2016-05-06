#include "bgen_casa_validation.h"

namespace bgen {
    namespace casa {
        
        namespace validation {

            shared_ptr < type > type_check (const type_info::shared & native_type, casa::type_map & dest) {

                auto type_it = dest.types.find (native_type->native_type_name());
                if (type_it != dest.types.end ())
                    return type_it->second;

                js_type jst = js_type_from_native(native_type);

                if (jst == js_type::unknown)
                    return nullptr;

                auto type_inst = make_shared < type > ();
                dest.types [native_type->native_type_name()] = type_inst;

                type_inst->js = jst;
                type_inst->native_type = native_type;

                if (jst == js_type::array) {
                    type_inst->array_type = type_check (native_type->template_params()[0].type(), dest);
                }

                if (jst == js_type::object) {
                    type_inst->structure = struct_check (native_type->struct_info(), dest);
                }

                return type_inst;
            }

            shared_ptr < structure > struct_check (const struct_info::shared & src, casa::type_map & dest) {

                casa::id_t id = { src->name (), src->namespace_name () };

                auto struct_it = dest.structures.find (id);
                if (struct_it != dest.structures.end ())
                    return struct_it->second;

                auto struct_inst = make_shared < structure > ();

                struct_inst->id = id;
                struct_inst->native_struct = src;

                dest.structures [struct_inst->id] = struct_inst;

                for ( auto & f : src->fields ()) {
                    if (f.visibility() != visibility_type::visibility_public)
                        continue;

                    shared_ptr < type > ftype = type_check (f.type (), dest);

                    if (!ftype) {
                        logger::write(f.location()) << "\"" << src->name () << ":" << f.name () << "\" unsupported type";
                        continue;
                    }

                    struct_inst->fields.push_back ({
                        f.name (),
                        ftype
                    });
                }

                return struct_inst;
            }

            void service_check (const method_info & src, casa::type_map & dest) {

                bool is_supported = true;
                auto service_inst = make_shared < service > ();

                service_inst->id = { src.name (), src.namespace_name() };
                service_inst->native_method = src;

                service_inst->return_type = type_check (src.return_type(), dest);
                if (!service_inst->return_type) {
                    logger::write (src.location()) << "\"" << src.name () << "\" unsuported return type";
                    is_supported = false;
                }

                if (src.params ().size () == 1) {
                    service_inst->param_type = type_check (src.params()[0].type(), dest);

                    if (!service_inst->param_type) {
                        logger::write (src.location()) << "\"" << src.name () << "\" unsuported argument type";
                        is_supported = false;
                    }
                } else if (src.params ().size () > 1){
                    logger::write (src.location ()) << "\"" << src.name () << "\" too many arguments, expected zero or one";
                    is_supported = false;
                }

                if (is_supported)
                    dest.services.push_back (service_inst);
            }

            void validate_and_set ( const bgen::type_map & src, casa::type_map & dest) {
                auto & structs = src.sorted_structs ();

                for (auto & s : structs) {
                    if (s->is_visited())
                        struct_check (s, dest);
                }

                for (auto & m : src.global()->methods()) {
                    service_check (m, dest);
                }
            }
        }
    }
}