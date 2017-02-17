
#ifndef user_case_state_p_hpp
#define user_case_state_p_hpp

#include "../state.hpp"
#include "state_a.hpp"

struct state_p
    : public sm::basic_state<state_p>
    , public sm::slot<state_a>
{
    void on_enter();
    void on_exit();
};

#endif // user_case_state_p_hpp

