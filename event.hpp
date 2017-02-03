
#ifndef EVENT_HPP
#define EVENT_HPP

#include <list>
#include <cassert>
#include <initializer_list>
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

    bool has_caller()
    {
        return(nullptr != m_caller);
    }

    void delete_caller()
    {
        if(has_caller())
            delete m_caller;
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

template <typename t_context_crt, typename t_event>
struct handles
{
    bool m_caller_set;

    handles()
        : m_caller_set(false)
    {
        event_list<t_event>* evt_list = event_list<t_event>::auto_instance();

        if(! evt_list->has_caller())
        {
//            std::cout << "setting caller : " << __PRETTY_FUNCTION__ << std::endl;
            evt_list->set_caller(new event_caller<t_context_crt, t_event>(static_cast<t_context_crt*>(this)));
            m_caller_set = true;
        }
    }

    virtual ~handles()
    {
        if(m_caller_set)
        {
//            std::cout << "freeing caller : " << __PRETTY_FUNCTION__ << std::endl;
            event_list<t_event>::auto_instance()->delete_caller();
        }

    }
};

struct master_list
{
    std::list<i_event_list*> m_event_lists;
    std::list<i_event_list*> m_singletons_to_free;

    master_list()
        : m_event_lists()
        , m_singletons_to_free()
    {
    }

    ~master_list()
    {
//        for(auto it=m_singletons_to_free.begin(); it!=m_singletons_to_free.end(); ++it)
//            delete (*it);
//
//        m_singletons_to_free.clear();
    }

    template <typename t_event>
    void declare()
    {
        m_singletons_to_free.push_back(event_list<t_event>::auto_instance());
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

#endif // EVENT_HPP
