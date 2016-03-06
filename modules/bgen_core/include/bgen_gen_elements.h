#pragma once
#ifndef _bgen_gen_elements_h_
#define _bgen_gen_elements_h_

#include <cinttypes>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

namespace bgen {
    namespace gen {
        
        struct indent {
            
            string   white_spaces;
            uint32_t spaces = 4;
            uint32_t level  = 0;
            
            inline void update () {
                white_spaces = string (level * spaces, ' ');
            }
            
            inline void operator ++ () {
                ++level;
                update ();
            }
            
            inline void operator -- () {
                if (level > 0) {
                    --level;
                    update ();
                }
            }
        };
        
        namespace details {
            struct line_builder;
        }
        
        struct output {
            ostream &   stream;
            indent      indent;

            inline details::line_builder line ();
        };
        
        namespace details {
            struct line_builder {
                
                output & out;
                bool has_copy;
                
                template < class _t >
                inline line_builder & operator << (const _t & v) {
                    out.stream << v;
                    return *this;
                }
                
                inline line_builder (output & out_r) :
                out (out_r),
                has_copy (false)
                {}
                
                inline line_builder (line_builder && origin) :
                out (origin.out),
                has_copy (false)
                {
                    origin.has_copy = true;
                }
                
                virtual ~line_builder() {
                    if (!has_copy) {
                        out.stream << endl;
                    }
                }
            };
        }
        
        details::line_builder output::line () {
            stream << indent.white_spaces;
            
            return {
                *this
            };
        }
        
        class element_base {
        private:

        public:

            using shared = shared_ptr < element_base >;

            virtual void write (output & out) const = 0;

        };

        class text : public element_base {
        public:

            string content;

            text () = default;
            text ( const string & content_v);

            using shared = shared_ptr < text > ;

            virtual void write (output & out) const override;

        };

        class group : public element_base {
        private:
            vector < element_base::shared > _content;
        public:
            
            using shared = shared_ptr < group >;

            template < class element_t, class ... ctor_args_tv >
            inline shared_ptr < element_t > make_item (ctor_args_tv && ... ctor_args_v) {
                auto item = make_shared < element_t > (ctor_args_v...);
                _content.push_back (item);
                return item;
            }

            virtual void write (output & out) const override;

        };

    }
}

#endif