
#ifndef user_case_state_p_hpp
#define user_case_state_p_hpp

#include "../state.hpp"
#include "state_a.hpp"

struct state_p
    : public sm::basic_state<state_p>
    , public sm::slot<state_a>
{
    state_p()            {std::cout << "state P ctor" << std::endl;}
    virtual ~state_p()   {std::cout << "state P dtor" << std::endl;}
    void on_enter()      {std::cout << "state P .enter" << std::endl;}
    void on_exit()       {std::cout << "state P .exit" << std::endl;}
};

#endif // user_case_state_p_hpp

