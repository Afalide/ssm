
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

struct entry_state
    : public sm::basic_state<entry_state>
{
    int init;
    entry_state() {}
    virtual ~entry_state(){}
    void on_enter(){}
    void on_exit(){}
};

struct master_state
    : public sm::basic_state<master_state>
    , public sm::slot<entry_state>
{
    master_state(){}
    virtual ~master_state(){}
    void on_enter(){}
    void on_exit(){}
};

int main()
{
    master_state master;
    sm::i_slot* master_slot = static_cast<sm::slot<entry_state>*>(&master);

    sm::state_holder<entry_state>* entry_hld = static_cast<sm::state_holder<entry_state>*>(master_slot->m_state_holder);
    entry_state* entry_st = entry_hld->m_state;

    entry_st->transit<state1>();



//    master_state<state1> main_state;
//    state1 s1;
//    s1.transit<null_state>();

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

