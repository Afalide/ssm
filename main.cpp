
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

//struct dummy_state
//    : public sm::basic_state<dummy_state>
//
//{
//    void on_enter(){std::cout << "enter dummy state" << std::endl;}
//    void on_exit(){std::cout << "exit dummy state" << std::endl;}
//};

static master_list g_events;

template <typename t_start_state>
struct state_launcher
{
    t_start_state* m_state;
    master_list m_events;
    sm::i_slot* m_slot;

    struct ev_go{};

    struct dummy_state
        : public sm::basic_state<dummy_state>
        , public handles<dummy_state, ev_go>
    {
        dummy_state(){std::cout << __FUNCTION__ << std::endl;}
        ~dummy_state(){std::cout << __FUNCTION__ << std::endl;}
        void on_enter(){std::cout << "enter dummy state" << std::endl;}
        void on_exit(){std::cout << "exit dummy state" << std::endl;}
        void handle(ev_go)
        {
            std::cout << "dummy_state : ev_go" << std::endl;


//            this->template transit<t_start_state>();
        }
    };

    state_launcher()
    {
        m_slot = new sm::slot<t_start_state>;
        m_slot->fw_islot_on_enter();
//        g_events.post(ev_go());

//        m_slot->switch_holder<t_start_state>(true);
//        m_slot->fw_islot_on_enter();
    }

    ~state_launcher()
    {
        m_slot->fw_islot_on_exit();
        delete m_slot;
//        m_slot->switch_holder<dummy_state>(true);
    }
};


int main()
{
    std::cout << "----------------------------- main: begin" << std::endl;

    auto launcher = new state_launcher<state_p>;

    std::cout << "----------------------------- main: events" << std::endl;

    for(int i=0; i<10; ++i)
        g_events.process_next();

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

