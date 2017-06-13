
#ifndef user_case_state_a1_hpp
#define user_case_state_a1_hpp

#include "../state.hpp"
#include "../event.hpp"

struct state_a;

struct ev_a1_foo {};

struct state_a1
    : public sm::basic_state<state_a1, state_a>
    , public handles<state_a1, ev_a1_foo>
{
    void on_enter();
    void on_exit();
    void handle(ev_a1_foo);
};

#endif
