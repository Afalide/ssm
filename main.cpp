
#include <iostream>
#include "event.hpp"
#include "slot.hpp"
#include "user_cases/state_p.hpp"
#include "user_cases/state_a1.hpp"

static master_list* g_events = nullptr;

template <typename t_start_state>
struct state_launcher
{
    sm::i_slot* m_slot;

    state_launcher()
    {
        m_slot = new sm::slot<t_start_state>;
        m_slot->fw_islot_on_enter();
    }

    ~state_launcher()
    {
        m_slot->fw_islot_on_exit();
        delete m_slot;
    }
};

struct eva{};
struct evb{};
struct evc{};
struct evd{};
struct eve{};

struct myobserver
    : public handles<myobserver, eva, evb, evc>
{
    void handle(eva) {std::cout << "myobserver eva" << std::endl;}
    void handle(evb) {std::cout << "myobserver evb" << std::endl;}
    void handle(evc) {std::cout << "myobserver evc" << std::endl;}
};

struct myotherobserver
    : public handles<myotherobserver, eva, evd, eve>
{
    void handle(eva) {std::cout << "myobserver eva" << std::endl;}
    void handle(evd) {std::cout << "myobserver evd" << std::endl;}
    void handle(eve) {std::cout << "myobserver eve" << std::endl;}
};

struct anotherobserver
    : public handles <anotherobserver, eva, evb, evc, evd, eve>
{
    void handle(eva) {std::cout << "anotherobserver eva" << std::endl;}
    void handle(evb) {std::cout << "anotherobserver evb" << std::endl;}
    void handle(evc) {std::cout << "anotherobserver evc" << std::endl;}
    void handle(evd) {std::cout << "anotherobserver evd" << std::endl;}
    void handle(eve) {std::cout << "anotherobserver eve" << std::endl;}
};

int main()
{
    std::cout << "----------------------------- main: begin" << std::endl;

    g_events = new master_list;

    myobserver obs;
    myotherobserver oobs;
    anotherobserver aobs;
    auto launcher = new state_launcher<state_p>;
    g_events->post<>(ev_a1_foo());

    std::cout << "----------------------------- main: events" << std::endl;

    for(int i=0; i<10; ++i)
        g_events->process_next();

    std::cout << "----------------------------- main: delete" << std::endl;

    delete launcher;
    delete g_events;

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

