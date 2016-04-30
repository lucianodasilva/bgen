#pragma once
#ifndef _bgen_casa_cpp_generation_h_
#define _bgen_casa_cpp_generation_h_

#include <bgen_core.h>
#include <string>

using namespace std;

namespace bgen {
    namespace casa {
        
        namespace gen {
            
            namespace cpp {

                string method_listener_name(const method_info &info, const string &rest_method);

                class parser_reader_pre : public bgen::gen::element_base {
                private:
                    struct_info::shared _info;
                public:

                    parser_reader_pre(const struct_info::shared &info);

                    virtual void write(bgen::gen::output &out) const override;

                };

                class parser_reader : public bgen::gen::element_base {
                private:
                    struct_info::shared _info;
                public:

                    parser_reader(const struct_info::shared &info);

                    virtual void write(bgen::gen::output &out) const override;

                };

                class parser_writer_pre : public bgen::gen::element_base {
                private:
                    struct_info::shared _info;
                public:

                    parser_writer_pre(const struct_info::shared &info);

                    virtual void write(bgen::gen::output &out) const override;

                };

                class parser_writer : public bgen::gen::element_base {
                private:
                    struct_info::shared _info;
                public:

                    parser_writer(const struct_info::shared &info);

                    virtual void write(bgen::gen::output &out) const override;

                };

                class listener_post : public bgen::gen::element_base {
                private:
                    method_info _info;
                public:

                    listener_post(const method_info &info);

                    virtual void write(bgen::gen::output &out) const override;

                };

                class listener_get : public bgen::gen::element_base {
                private:
                    method_info _info;
                public:

                    listener_get(const method_info &info);

                    virtual void write(bgen::gen::output &out) const override;

                };

                class register_listeners : public bgen::gen::element_base {
                private:
                    struct_info::shared _info;
                public:

                    register_listeners(const struct_info::shared &info);

                    virtual void write(bgen::gen::output &out) const override;

                };

                void generate (
                    bgen::type_map & types,
                    const string & output_file_name,
                    const string & parser_boilerplate_location,
                    const string & listener_boilerplate_location
                );

            }
        }
    }
}

#endif