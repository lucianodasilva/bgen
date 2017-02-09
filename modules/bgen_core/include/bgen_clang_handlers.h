//
// Created by Luciano Silva on 15/12/16.
//

#pragma once
#ifndef _bgen_clang_handlers_h_
#define _bgen_clang_handlers_h_

#include <clang-c/Index.h>
#include "bgen_details.h"

#include <map>

using namespace std;

namespace bgen {

    struct visitor_context;

    // namespace clang {
    //     namespace handlers {

    //         class cursor_type_handler {
    //         public:
    //             virtual void visit(visitor_context &cxt, const CXCursor &cursor);
    //             virtual void visit_post(visitor_context &cxt, const CXCursor &cursor);
    //         };

    //         class namespace_handler : public cursor_type_handler {
    //         public:
    //             virtual void visit(visitor_context &cxt, const CXCursor &cursor) override;
    //             virtual void visit_post(visitor_context &cxt, const CXCursor &cursor) override;
    //         };

    //         class field_handler : public cursor_type_handler {
    //         public:
    //             virtual void visit(visitor_context &cxt, const CXCursor &cursor) override;
    //         };

    //         class param_handler : public cursor_type_handler {
    //         public:
    //             virtual void visit(visitor_context &cxt, const CXCursor &cursor) override;
    //         };

    //         class method_handler_base {
    //         protected:
    //             void visit_method(visitor_context &cxt, const CXCursor &cursor, bool is_constructor);
    //             void visit_method_post(visitor_context &cxt, const CXCursor &cursor, bool is_constructor);
    //         };

    //         class ctor_method_handler : public method_handler_base, public cursor_type_handler {
    //         public:
    //             virtual void visit(visitor_context &cxt, const CXCursor &cursor) override;
    //             virtual void visit_post(visitor_context &cxt, const CXCursor &cursor) override;
    //         };

    //         class simple_method_handler : public method_handler_base, public cursor_type_handler {
    //         public:
    //             virtual void visit(visitor_context &cxt, const CXCursor &cursor) override;
    //             virtual void visit_post(visitor_context &cxt, const CXCursor &cursor) override;
    //         };

    //         class struct_def_handler_base {
    //         protected:
    //             void visit_struct(visitor_context &cxt, const CXCursor &cursor, struct_type type);
    //             void visit_struct_post(visitor_context &cxt, const CXCursor &cursor, struct_type type);
    //         };

    //         class struct_def_handler : public struct_def_handler_base, public cursor_type_handler {
    //         public:
    //             virtual void visit(visitor_context &cxt, const CXCursor &cursor) override;
    //             virtual void visit_post(visitor_context &cxt, const CXCursor &cursor) override;
    //         };

    //         class class_def_handler : public struct_def_handler_base, public cursor_type_handler {
    //         public:
    //             virtual void visit(visitor_context &cxt, const CXCursor &cursor) override;
    //             virtual void visit_post(visitor_context &cxt, const CXCursor &cursor) override;
    //         };

    //         class struct_base_def_handler : public cursor_type_handler {
    //         public:
    //             virtual void visit(visitor_context &cxt, const CXCursor &cursor) override;
    //         };

    //         using handler_map_t = map < CXCursorKind, unique_ptr < cursor_type_handler > >;

    //         struct lookup {

	// 			handler_map_t handlers;
	// 			unique_ptr<cursor_type_handler> null_handler;

    //         };

    //         cursor_type_handler & lookup_get (const lookup & ltable, CXCursorKind kind);

    //         lookup lookup_make_default ();
    //     }
    // }
}

#endif //_bgen_clang_handlers_h_
