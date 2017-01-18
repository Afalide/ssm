
#ifndef client_state_a_hpp
#define client_state_a_hpp

#include "state.hpp"
#include "client_events.hpp"
#include "client_state_b.hpp"
#include "client_state_c.hpp"

//struct state2;

struct state1
//    : public state<state1, state2>
    : public sm::basic_state<state1>
    , public sm::slot<state2>
    , public sm::slot<state3>
{
    state1()
    {
        std::cout << "ctor state1" << std::endl;
    }

    virtual ~state1()
    {
        std::cout << "dtor state1" << std::endl;
    }

    void on_enter()
    {
        std::cout << "enter state1" << std::endl;
    }

    void on_exit()
    {
        std::cout << "exit state1" << std::endl;
    }
};

#endif // client_state_a_hpp

