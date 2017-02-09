
#include <iostream>
#include "event.hpp"
//#include "state.hpp"
//#include "client_state_a.hpp"
//#include "client_state_c.hpp"

#include "slot.hpp"
#include "user_cases/state_p.hpp"

//struct eva
//{
////    eva(){std::cout << "eva ctor" << std::endl;}
////    eva(const eva&){std::cout << "eva ctor copy" << std::endl;}
//};
//
//struct evb{};
//struct evc{};


//template <typename t_start_state>
//struct master_state
//{
//    t_start_state* m_start_state;
//
//    master_state()
//        : m_start_state(nullptr)
//    {
//        m_start_state = new t_start_state;
////        m_start_state->enter_hierarchy();
////        m_start_state = enter_in<t_start_state>();
//    }
//
//    ~master_state()
//    {
////        m_start_state->exit_hierarchy();
//        delete m_start_state;
////        exit_from<>(m_start_state);
//    }
//};

//struct ev_go{};
//
//struct entry_state
//    : public sm::basic_state<entry_state>
//    , public handles<entry_state, ev_go>
//{
//    virtual ~entry_state(){}
//    void on_enter(){}
//    void on_exit(){}
//
//    void handle(ev_go)
//    {
//        transit<state1>();
//    }
//};
//
//struct master_state
//    : public sm::basic_state<master_state>
//    , public sm::slot<
//{
//    virtual ~master_state(){}
//    void on_enter(){}
//    void on_exit(){}
//};

template <typename t_start_state>
struct state_launcher
{
    typedef sm::state_holder<t_start_state> t_holder;
    t_holder* m_holder;

    state_launcher()
    {
        m_holder = new t_holder;
        m_holder->forward_on_enter();
        m_holder->forward_on_enter_on_slots();
    }

    ~state_launcher()
    {
        m_holder->forward_on_exit_on_slots();
        m_holder->forward_on_exit();
        delete m_holder;
    }
};


int main()
{
    std::cout << "----------------------------- main: begin" << std::endl;

    auto launcher = new state_launcher<state_p>;

    std::cout << "----------------------------- main: delete" << std::endl;

    delete launcher;

    std::cout << "----------------------------- main: done" << std::endl;

////////////////////////////////////////////////////////////////////

//    master_list master;
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

