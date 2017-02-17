
#ifndef user_case_state_a_hpp
#define user_case_state_a_hpp

#include "../state.hpp"
#include "state_a1.hpp"

struct state_a
    : public sm::basic_state<state_a>
    , public sm::slot<state_a1>
{
    void on_enter();
    void on_exit();
};

#endif // user_case_state_a_hpp


