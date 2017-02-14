
#ifndef user_case_state_b_hpp
#define user_case_state_b_hpp

#include "../state.hpp"

struct state_a
    : public sm::basic_state<state_b>
//    , public sm::slot<
{
    state_b()            {std::cout << "state B ctor" << std::endl;}
    virtual ~state_b()   {std::cout << "state B dtor" << std::endl;}
    void on_enter()      {std::cout << "state B  ..enter" << std::endl;}
    void on_exit()       {std::cout << "state B  ..exit" << std::endl;}
};

#endif // user_case_state_b_hpp


