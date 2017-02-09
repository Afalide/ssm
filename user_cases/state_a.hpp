
#ifndef user_case_state_a_hpp
#define user_case_state_a_hpp

#include "../state.hpp"

struct state_a
    : public sm::basic_state<state_a>
//    , public sm::slot<
{
    state_a()            {std::cout << "state A ctor" << std::endl;}
    virtual ~state_a()   {std::cout << "state A dtor" << std::endl;}
    void on_enter()      {std::cout << "state A ..enter" << std::endl;}
    void on_exit()       {std::cout << "state A ..exit" << std::endl;}
};

#endif // user_case_state_a_hpp


