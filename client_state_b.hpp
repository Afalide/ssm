
#ifndef client_state_hpp
#define client_state_hpp

#include "state.hpp"
//#include "client_state_a.hpp"

//struct state1;

struct state2
//    : public state<state2, null_state, state1>
    : public sm::basic_state<state2>
    , public handles<state2, eva>
{
    state2()
    {
        std::cout << "ctor state2" << std::endl;
    }

    virtual ~state2()
    {
        std::cout << "dtor state2" << std::endl;
    }

    void on_enter()
    {
        std::cout << "enter state2" << std::endl;
    }

    void on_exit()
    {
        std::cout << "exit state2" << std::endl;
    }

    void handle(eva)
    {
        std::cout << "state2 handle eva" << std::endl;
    }
};

#endif // client_state_hpp
