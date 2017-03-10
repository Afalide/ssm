
#include <iostream>
#include "event.hpp"
//#include "state.hpp"
#include "client_state_a.hpp"
#include "client_state_c.hpp"

//struct eva
//{
////    eva(){std::cout << "eva ctor" << std::endl;}
////    eva(const eva&){std::cout << "eva ctor copy" << std::endl;}
//};
//
//struct evb{};
//struct evc{};
//
//struct state2;

//struct state1
//    : public state<state1,state2>
//{
//    state1()
//    {
////        event_list<eva>::auto_instance()->set_caller(new event_caller<state1, eva>(this));
////        event_list<evb>::auto_instance()->set_caller(new event_caller<state1, evb>(this));
//        std::cout << "state1 ctor" << std::endl;
//    }
//
//    virtual ~state1()
//    {
//        std::cout << "state1 dtor" << std::endl;
//    }
//
//    void on_enter()
//    {
//        std::cout << "enter state1" << std::endl;
//    }
//
//    void on_exit()
//    {
//        std::cout << "exit state1" << std::endl;
//    }
//
//    void handle(const eva&){std::cout << "state1 - eva" << std::endl;}
//    void handle(evb){std::cout << "state1 - evb" << std::endl;}
//};

//struct state2
//    : public state<state2>
//{
//    state2()
//    {
//        std::cout << "state2 ctor" << std::endl;
//    }
//
//    virtual ~state2()
//    {
//        std::cout << "state2 dtor" << std::endl;
//    }
//
//    void on_enter()
//    {
//        std::cout << "enter state2" << std::endl;
//    }
//
//    void on_exit()
//    {
//        std::cout << "exit state2" << std::endl;
//    }
//};
//
//struct state3
//{
//
//};

struct master_list
{
    std::list<i_event_list*> m_event_lists;

    master_list()
        : m_event_lists()
    {
    }

    template <typename t_event>
    void post(const t_event& ev)
    {
        event_list<t_event>::auto_instance()->add(ev);
        m_event_lists.push_back(event_list<t_event>::auto_instance());
    }

    void process_next()
    {
        if(m_event_lists.size() == 0)
        {
            std::cout << "no list available (no event posted in any list)" << std::endl;
            return;
        }

        i_event_list* evlist = m_event_lists.front();
        m_event_lists.pop_front();
        evlist->process_next();
    }
};

template <typename t_start_state>
struct master_state
{
    t_start_state* m_start_state;

    master_state()
        : m_start_state(nullptr)
    {
//        m_start_state = new t_start_state;
//        m_start_state->enter_hierarchy();
        m_start_state = enter_in<t_start_state>();
    }

    ~master_state()
    {
//        m_start_state->exit_hierarchy();
//        delete m_start_state;
        exit_from<>(m_start_state);
    }
};

int main()
{
    master_state<state1> main_state;

//    main_state.m_start_state->m_child_state->transit<state3>();

//    state1 s1;
//
////    s1.
//
//    master_list master;
//
////
////
//    master.post(eva());
//    master.post(evb());
//    master.post(evb());
//
//    master.process_next();
//    master.process_next();
//    master.process_next();
//    master.process_next();
//    master.process_next();

//    event_list<eva>::auto_instance()->process_next();
//    event_list<evb>::auto_instance()->process_next();
//    event_list<evb>::auto_instance()->process_next();

    return 0;
}

