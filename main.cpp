
#include <iostream>
#include "event.hpp"
#include "slot.hpp"
#include "user_cases/state_p.hpp"

static master_list g_events;

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

