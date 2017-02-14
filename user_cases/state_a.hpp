
#ifndef user_case_state_a_hpp
#define user_case_state_a_hpp

#include "../state.hpp"

struct state_a1;

struct state_a
    : public sm::basic_state<state_a>
    , public sm::slot<state_a1>
{
    state_a()            {std::cout << "state A ctor" << std::endl;}
    virtual ~state_a()   {std::cout << "state A dtor" << std::endl;}
    void on_enter()      {std::cout << "state A  ..enter" << std::endl;}
    void on_exit()       {std::cout << "state A  ..exit" << std::endl;}
};

struct state_a2;

struct state_a1
    : public sm::basic_state<state_a1>
{
    state_a1()            {std::cout << "state A1 ctor" << std::endl;}
    virtual ~state_a1()   {std::cout << "state A1 dtor" << std::endl;}
    void on_enter()      {std::cout << "state A1 ...enter" << std::endl; transit<state_a2>();}
    void on_exit()       {std::cout << "state A1 ...exit" << std::endl;}
};

struct ev_a2{};

struct state_a2
    : public sm::basic_state<state_a2>
//    , public handles<state_a2, ev_a2>
{
    state_a2()            {std::cout << "state A2 ctor" << std::endl;}
    virtual ~state_a2()   {std::cout << "state A2 dtor" << std::endl;}
    void on_enter()      {std::cout << "state A2 ...enter" << std::endl;}
    void on_exit()       {std::cout << "state A2 ...exit" << std::endl;}
};

#endif // user_case_state_a_hpp


