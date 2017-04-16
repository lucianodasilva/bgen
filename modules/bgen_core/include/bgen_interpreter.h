#pragma once
#ifndef _bgen_interpreter_h_
#define _bgen_interpreter_h_

#include "bgen_common.h"

#include <functional>
#include <unordered_map>

namespace bgen {

    namespace interpreter_details {

        template < class _k_t, class _action_signature_t >
        class storage {
        public:
            using key_type = _k_t;
            using action_type = function < _action_signature_t >;
        private:
            unordered_map < key_type, action_type > _actions;
            action_type _default_action;
        public:

            inline void set_default_action (const action_type & action) {
                _default_action = action;
            }	

            inline storage & add_action (const key_type & key, const action_type & action) {
                _actions[key] = move(action);
                return *this;
            }

            inline const action_type & get_action (const key_type & k) const {
                auto it = _actions.find (k);
                if (it == _actions.end())
                    return _default_action;
                else
                    return it->second;
            }
        };

        template < class _k_t, class _action_signature >
        class executor;

        template < class _k_t, class _r_t, class ... _args_t >
        class executor < _k_t, _r_t (_args_t ...) > : public storage < _k_t, _r_t (_args_t ...) > {
        public:
            inline _r_t execute (const typename executor::key_type & key, _args_t ... args) const{
                auto & action = executor::get_action (key);
                if (action)
                    return action (args...);
                else
                    return _r_t();
            }
        };

        template <class _k_t, class ... _args_t >
        class executor < _k_t, void (_args_t ...) > : public storage < _k_t, void (_args_t ...) > {
        public:
            inline void execute (const typename executor::key_type & key, _args_t ... args) const{
                auto & action = executor::get_action (key);
                if (action)
                    action (args...);
            }
        };
    }

    template < class _k_t, class _action_signature_t >
    class interpreter : public interpreter_details::executor < _k_t, _action_signature_t > {};

}

#endif