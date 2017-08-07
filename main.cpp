
#include <mm/state.hpp>
#include <mm/observer.hpp>

struct state_a;
struct state_b;
struct state_b1;
struct state_b2;
struct state_c;

struct evt_a {};

struct boss
    : public mm::state<boss>
    , public mm::slot<state_a, state_b, state_c>
    , public mm::handles<boss, evt_a>
{
    boss(){std::cout << "enter " << __FUNCTION__ << std::endl;}
    virtual ~boss(){std::cout << "exit " << __FUNCTION__ << std::endl;}
    void handle (evt_a)
    {
        std::cout << "boss state evt a" << std::endl;
    }
};

struct state_a : public mm::state<state_a>
{
    state_a(){std::cout << "enter " << __FUNCTION__ << std::endl;}
    virtual ~state_a(){std::cout << "exit " << __FUNCTION__ << std::endl;}
};

struct state_b : public mm::state<state_b>
               , public mm::slot<state_b1, state_b2>
{
    state_b(){std::cout << "enter " << __FUNCTION__ << std::endl;}
    virtual ~state_b(){std::cout << "exit " << __FUNCTION__ << std::endl;}
};

struct state_b1 : public mm::state<state_b1>
{
    state_b1(){std::cout << "enter " << __FUNCTION__ << std::endl;}
    virtual ~state_b1(){std::cout << "exit " << __FUNCTION__ << std::endl;}
};

struct state_b2 : public mm::state<state_b2>
{
    state_b2(){std::cout << "enter " << __FUNCTION__ << std::endl;}
    virtual ~state_b2(){std::cout << "exit " << __FUNCTION__ << std::endl;}
};

struct state_c : public mm::state<state_c>
{
    state_c(){std::cout << "enter " << __FUNCTION__ << std::endl;}
    virtual ~state_c(){std::cout << "exit " << __FUNCTION__ << std::endl;}
};

struct state_d : public mm::state<state_d>
{
	state_d() { std::cout << "enter " << __FUNCTION__ << std::endl; }
	virtual ~state_d() { std::cout << "exit " << __FUNCTION__ << std::endl; }
};

int main()
{
    std::cout << "main begin" << std::endl;

    mm::state_machine<boss> sm;

	boss* state_boss = dynamic_cast<boss*> (sm.m_states[0]);
	state_b* sb = dynamic_cast<state_b*>(state_boss->m_states[1]);

	sb->go<state_d>();

    mm::master_list mlist;
    mlist.post(evt_a());
    mlist.process_next();

    std::cout << "main end" << std::endl;
    return 0;
}
