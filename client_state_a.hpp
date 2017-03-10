
#ifndef client_state_a_hpp
#define client_state_a_hpp

#include "state.hpp"
#include "client_events.hpp"
#include "client_state_b.hpp"

//struct state2;

struct state1
    : public state<state1, state2>
{
    state1()
    {
        std::cout << "ctor state1" << std::endl;
    }

    virtual ~state1()
    {
        std::cout << "dtor state2" << std::endl;
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

