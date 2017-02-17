
#ifndef user_case_state_a1_hpp
#define user_case_state_a1_hpp

#include "../state.hpp"

struct state_a;

struct state_a1
    : public sm::basic_state<state_a1, state_a>
{
    void on_enter();
    void on_exit();
};

#endif
