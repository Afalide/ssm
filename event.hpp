
#ifndef EVENT_HPP
#define EVENT_HPP

#include <list>
#include <cassert>
#include "auto_singleton.hpp"

template <typename t_event>
struct i_event_caller
{
    virtual ~i_event_caller(){}
    virtual void process(const t_event& ev) = 0;
//    virtual void handle_fed() = 0;
};

template <typename t_context, typename t_event>
struct event_caller
     : public i_event_caller<t_event>
{
    t_context* m_context;
//    t_event m_event;

    event_caller(t_context* ctx)
        : m_context(ctx)
//        , m_event()
    {
    }

    virtual ~event_caller(){}

    virtual void process (const t_event& ev) override
    {
        m_context->handle(ev);
    }
};

struct i_event_list
{
    virtual ~i_event_list(){}
    virtual void process_next() = 0;
};

template <typename t_event>
struct event_list
    : public i_event_list
    , public auto_singleton<event_list<t_event>>
{
    std::list<t_event*> m_events;
    i_event_caller<t_event>* m_caller;

    event_list() : m_events(), m_caller(nullptr) {}

    void set_caller(i_event_caller<t_event>* caller)
    {
        m_caller = caller;
    }

    void add(const t_event& ev)
    {
        m_events.push_back(new t_event(ev));
    }

    void process_next() override
    {
//        std::cout << __PRETTY_FUNCTION__ << std::endl;
//        assert(nullptr != m_caller); // You must invoke set_caller() before processing this event list

        if(nullptr == m_caller)
        {
            std::cout << "no caller set for this event list" << std::endl;
            return;
        }

        if(m_events.size() == 0)
        {
            std::cout << "no events in this list" << std::endl;
            return;
        }

        t_event* ev = m_events.front();
        m_events.pop_front();
        m_caller->process(*ev);
        delete ev;
    }
};

#endif // EVENT_HPP
