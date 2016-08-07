
#include <iostream>
#include <thread>
#include <context.hpp>
#include <state.hpp>
#include <event_processor.hpp>
#include <tests/tests.hpp>

struct ev_simple{};
struct ev_request_transit{};
struct ev_request_processor_stop{};

struct i_state;

struct my_context
    : public context<i_state>
{
};

struct i_state
    : public state<i_state>
{
    int m_value;

    i_state() : m_value(0) {}
    virtual ~i_state(){}

    virtual int get_id() = 0;

    virtual int handle(ev_simple)                   {return event_done();}
    virtual int handle(ev_request_transit)          {return event_done();}
    virtual int handle(ev_request_processor_stop)   {return event_done();}
};

my_context ctx;
mt_event_processor mt_events;
event_processor& events(mt_events);

struct state_do_stuff;

struct state_idle
    : public i_state
{
    virtual ~state_idle() {}

    virtual int get_id() override
    {
        return 1;
    }

    virtual int handle(ev_simple) override 
    {
        m_value = 10;
        return event_done();
    }

    virtual int handle(ev_request_transit) override
    {
        events.post_transit_event<my_context, state_do_stuff>(&ctx);
        return event_done();
    }
};

struct state_do_stuff
    : public i_state
{
    state_do_stuff() {}
    virtual ~state_do_stuff() {}

    virtual int get_id() override
    {
        return 2;
    }

    virtual int handle(ev_simple) override
    {
        m_value += 5;
        return event_done();
    }

    virtual int handle(ev_request_processor_stop) override
    {
        events.set_active(false);
        return event_done();
    }
};

void basic_tests()
{
    ctx.m_event_processor = &events;

    assert(0 == events.count());
    assert(events.is_active());
    assert(nullptr == ctx.m_current_state);

    events.post_transit_event<my_context, state_idle>(&ctx);
    events.handle_next();
    assert(nullptr != ctx.m_current_state);
    assert(1 == ctx.m_current_state->get_id());

    events.post_basic_event(&ctx, ev_simple());
    assert(1 == events.count());
    events.handle_next();
    assert(0 == events.count());
    assert(10 == ctx.m_current_state->m_value);

    events.post_basic_event(&ctx, ev_request_transit());
    assert(1 == events.count());
    events.handle_next();
    assert(1 == events.count());
    events.handle_next();
    assert(0 == events.count());
    assert(2 == ctx.m_current_state->get_id());

    events.post_basic_event(&ctx, ev_request_processor_stop());
    events.handle_next();
    assert(0 == events.count());
    assert(!events.is_active());
    assert(0 == ctx.m_current_state->m_value);
    events.post_basic_event(&ctx, ev_simple());
    events.post_basic_event(&ctx, ev_simple());
    events.post_basic_event(&ctx, ev_simple());
    events.handle_next();
    events.handle_next();
    events.handle_next();
    assert(3 == events.count());
    assert(0 == ctx.m_current_state->m_value);

    events.set_active(true);
    events.handle_next();
    events.handle_next();
    events.handle_next();
    assert(15 == ctx.m_current_state->m_value);

    assert(0 == events.count());
    i_state* current_state = ctx.m_current_state;
    assert(nullptr != current_state);
	current_state->post<ev_simple>(ev_simple());
    events.handle_next();
    assert(20 == current_state->m_value);
}

//////////////////////////////////////////////////////////////

struct i_state_main
    : public state <i_state_main>
{
    i_state_main() {}
    virtual ~i_state_main() {}
    virtual int handle(ev_simple) {return event_done();}
};

struct main_ctx
    : public context<i_state_main>
{
};

struct bar;

struct foo
    : public i_state_main
{
    foo(){}
    virtual ~foo(){}
    virtual int handle(ev_simple) override {return immediate_transit<bar>();}
};

struct i_substate
    : public state<i_substate>
{
    i_substate(){}
    virtual ~i_substate(){}
    virtual int handle(ev_simple) {return event_done();}
};

struct substate1;

struct bar
    : public i_state_main
    , public context<i_substate>
{
    bar(){perform_transit<substate1>();}
    virtual ~bar(){}
    virtual int handle(ev_simple) override {return forward_event();}
};

struct substate1
    : public i_substate
{
    substate1(){}
    virtual ~substate1(){}
    virtual int handle(ev_simple) override {return event_done();}
};

void sub_context_tests()
{
    
}


//
//#include <iostream>
//#include <thread>
//#include <context.hpp>
//#include <state.hpp>
//#include <event_processor.hpp>
//
//
//#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>
//
////st_event_processor events;
//mt_event_processor events;
//
//struct eva{};
//struct evb{};
//struct evc{};
//
//struct my_state;
//
//struct user_ctx 
//    : public context<my_state>
//{
//    user_ctx(){}
//    virtual ~user_ctx(){}
//};
//
//user_ctx ctx;
//
//struct my_state : public state<my_state>
//{
//    my_state(){}
//    virtual ~my_state(){}
//    virtual int handle(eva) {std::cout << "default handle eva" << std::endl; return event_done(); }
//    virtual int handle(evb) {std::cout << "default handle evb" << std::endl; return event_done(); }
//    virtual int handle(evc) {std::cout << "default handle evc" << std::endl; return event_done(); }
//};
//
//struct my_state_b;
//struct my_state_a : public my_state 
//{
//    my_state_a()           {std::cout << "enter state A" << std::endl;}
//    virtual ~my_state_a()  {std::cout << "exit  state A" << std::endl;}
//    virtual int handle(eva) override { std::cout << "state A handle eva override" << std::endl; 
//                                       events.post_transit<user_ctx, my_state_b>(&ctx); 
//                                       events.post(&ctx, evb());
//                                       events.post(&ctx, evc());
//                                       return event_done(); }
//};
//
//struct my_state_b : public my_state 
//{
//    my_state_b()           {std::cout << "enter state B" << std::endl;}
//    virtual ~my_state_b()  {std::cout << "exit  state B" << std::endl;}
//    virtual int handle(evb) override { std::cout << "state B handle evb override" << std::endl; 
//                                       events.set_active(false);
//                                       return event_done(); }
//
//    virtual int handle(evc) override { std::cout << "state B handle evc override" << std::endl; 
//                                       return immediate_transit<my_state_a>(); }
//};
//
////LRESULT CALLBACK my_keyboard_proc(int code, WPARAM w_param, LPARAM l_param)
////{
////    //required by msdn
////
////    if (code < 0)
////        return CallNextHookEx(NULL, code, w_param, l_param);
////
////    //handle the keyboard event
////
////    std::cout << "keyboard event" << std::endl;
////
////    return CallNextHookEx(NULL, code, w_param, l_param);
////}
//
//void send_events()
//{
//    Sleep(1000);
//    events.post(&ctx, eva());
//
//    //Sleep(1000);
//    //events.post(&ctx, eva());
//
//    //Sleep(1000);
//    //events.post(&ctx, evb());
//
//    //Sleep(1000);
//    //events.post(&ctx, evc());
//}
//
//int main()
//{
//    ctx.perform_transit<my_state_a>();
//
//    //events.post(&ctx, eva());
//    //events.post(&ctx, eva());
//    //events.post(&ctx, evb());
//    //events.post(&ctx, evc());
//
//    //events.handle_all();
//
//    //std::cout << "----" << std::endl;
//
//    std::thread thread_which_sends_events (send_events);
//
//    //system("pause");
//
//    while(events.is_active())
//    {
//        std::cout << "waiting for one event to handle..." << std::endl;
//        events.wait_handle_next();
//    }
//    
//    //ctx.current_state_handle(eva());
//    //ctx.current_state_handle(eva());
//    //ctx.current_state_handle(eva());
//    //ctx.set_current_state<my_state_b>();
//    
//    thread_which_sends_events.join();
//
//    return 0;
//}
