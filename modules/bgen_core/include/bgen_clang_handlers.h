//
// Created by Luciano Silva on 15/12/16.
//

#pragma once
#ifndef _bgen_clang_handlers_h_
#define _bgen_clang_handlers_h_

#include <clang-c/Index.h>
#include "bgen_details.h"
#include "bgen_visitor.h"

#include <map>

using namespace std;

namespace bgen {
    namespace clang {
        namespace handlers {

            class cursor_type_handler {
            public:
                virtual void visit_start (visitor_context & cxt, const CXCursor & cursor);
                virtual void visit_end (visitor_context & cxt, const CXCursor & cursor);
            };

            class namespace_handler : public cursor_type_handler {
            public:
                virtual void visit_start (visitor_context & cxt, const CXCursor & cursor) override;
                virtual void visit_end (visitor_context & cxt, const CXCursor & cursor) override;
            };

            class field_handler : public cursor_type_handler {
            public:
                virtual void visit_start (visitor_context & cxt, const CXCursor & cursor) override;
            };

            class param_handler : public cursor_type_handler {
            public:
                virtual void visit_start (visitor_context & cxt, const CXCursor & cursor) override;
            };

            class method_handler_base {
            protected:
                void method_visit_start (visitor_context & cxt, const CXCursor & cursor, bool is_constructor);
                void method_visit_end (visitor_context & cxt, const CXCursor & cursor, bool is_constructor);
            };

            class ctor_method_handler : public method_handler_base, public cursor_type_handler {
            public:
                virtual void visit_start (visitor_context & cxt, const CXCursor & cursor) override;
                virtual void visit_end (visitor_context & cxt, const CXCursor & cursor) override;
            };

            class simple_method_handler : public method_handler_base, public cursor_type_handler {
            public:
                virtual void visit_start (visitor_context & cxt, const CXCursor & cursor) override;
                virtual void visit_end (visitor_context & cxt, const CXCursor & cursor) override;
            };

            class struct_def_base_handler {
            protected:
                void struct_visit_start (visitor_context & cxt, const CXCursor & cursor, struct_type type);
                void struct_visit_end (visitor_context & cxt, const CXCursor & cursor, struct_type type);
            };

            class struct_def_handler : public struct_def_base_handler, public cursor_type_handler {
            public:
                virtual void visit_start(visitor_context &cxt, const CXCursor &cursor) override;
                virtual void visit_end(visitor_context &cxt, const CXCursor &cursor) override;
            };

            class class_def_handler : public struct_def_base_handler, public cursor_type_handler {
            public:
                virtual void visit_start(visitor_context &cxt, const CXCursor &cursor) override;
                virtual void visit_end(visitor_context &cxt, const CXCursor &cursor) override;
            };

            class struct_base_def_handler : public cursor_type_handler {
            public:
                virtual void visit_start(visitor_context &cxt, const CXCursor &cursor) override;
            };

            class lookup : no_copy {
            private:

                using handler_map_t = map < CXCursorKind, unique_ptr < cursor_type_handler > >;

                static handler_map_t init_map ();

                handler_map_t _handlers;
                unique_ptr < cursor_type_handler > _null_handler { make_unique < cursor_type_handler > () };

                lookup ();

            public:
                static cursor_type_handler & get (CXCursorKind kind);
            };

        }
    }
}

#endif //_bgen_clang_handlers_h_
