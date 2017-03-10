
#ifndef client_state_c_hpp
#define client_state_c_hpp

#include "state.hpp"
//#include "client_state_a.hpp"

struct state3
     : public state<state3, null_state, state1>
{
    state3()
    {
        std::cout << "ctor state3" << std::endl;
    }

    virtual ~state3()
    {
        std::cout << "dtor state3" << std::endl;
    }

    void on_enter()
    {
        std::cout << "enter state3" << std::endl;
    }

    void on_exit()
    {
        std::cout << "exit state3" << std::endl;
    }
};

#endif // client_state_c_hpp

