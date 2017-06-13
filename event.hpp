
#ifndef EVENT_HPP
#define EVENT_HPP

#include <list>
#include <cassert>
#include <vector>
#include <map>
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
    virtual void delete_caller() = 0;
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

    void delete_caller() override
    {
        if(has_caller())
        {
            delete m_caller;
            m_caller = nullptr;
        }
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

//template <typename t_context_crt, typename t_event>
//struct handles
//{
//    bool m_caller_set;
//
//    handles()
//        : m_caller_set(false)
//    {
//        event_list<t_event>* evt_list = event_list<t_event>::auto_instance();
//
//        if(! evt_list->has_caller())
//        {
//            evt_list->set_caller(new event_caller<t_context_crt, t_event>(static_cast<t_context_crt*>(this)));
//            m_caller_set = true;
//        }
//    }
//
//    virtual ~handles()
//    {
//        if(m_caller_set)
//        {
//            event_list<t_event>::auto_instance()->delete_caller();
//        }
//    }
//};

struct i_event_list_container
{
    std::vector<i_event_list*> m_event_lists;

    i_event_list_container()
        : m_event_lists()
    {
    }

    virtual ~i_event_list_container()
    {
    }

    void add(i_event_list* lst)
    {
        m_event_lists.push_back(lst);
    }

    void forward_delete_all_callers()
    {
        for(auto it=m_event_lists.begin(); it!=m_event_lists.end(); ++it)
            (*it)->delete_caller();
    }
};

//template <typename t_context, typename t_event >
//void declare_handle(t_context* ctx, i_event_list_container* cont)
//{
//    event_list<t_event>* evt_list = event_list<t_event>::auto_instance();
//    if(! evt_list->has_caller())
//    {
//        std::cout << "setting caller " << __PRETTY_FUNCTION__ << std::endl;
//        evt_list->set_caller(new event_caller<t_context, t_event>(ctx));
//        cont->add(evt_list);
//    }
//}

//template <typename t_context, typename t_event, typename... t_more_events>
//void declare_handle(t_context* ctx, i_event_list_container* cont)
//{
//    event_list<t_event>* evt_list = event_list<t_event>::auto_instance();
//
//    if(! evt_list->has_caller())
//    {
//        std::cout << "setting caller " << __PRETTY_FUNCTION__ << std::endl;
//        evt_list->set_caller(new event_caller<t_context, t_event>(ctx));
//        cont->add(evt_list);
//    }
//
//    declare_handle<t_context, t_more_events...>(ctx, cont);
//}


template <typename t_context, typename t_event>
void declare_handle_impl(t_context* context, i_event_list_container* container)
{
    std::cout << __PRETTY_FUNCTION__ << std::endl;

    event_list<t_event>* evt_list = event_list<t_event>::auto_instance();

    if(! evt_list-> has_caller())
    {
        std::cout << __PRETTY_FUNCTION__ << " setting caller" << std::endl;
        evt_list->set_caller(new event_caller<t_context, t_event>(context));
        container->add(evt_list);
        return;
    }

    std::cout << __PRETTY_FUNCTION__ << " a class is already registered as a caller for this event type" << std::endl;
}

template <typename t_context>
void declare_handle(t_context* /*context*/, i_event_list_container* /*container*/)
{
    // Called when no event can be extracted (the list was emptied)
    // Do nothing.
}

template <typename t_context, typename t_event, typename ... t_events>
void declare_handle(t_context* context, i_event_list_container* container)
{
    // Called when an event has been taken (extracted) from the list
    // Also called when the list is empty due to this extraction, which
    // is fine (the next call will fail to extract an event from the list,
    // see the function above).

    declare_handle_impl   <t_context, t_event>(context, container);
    declare_handle        <t_context, t_events ...>(context, container);
}

template <typename t_context, typename... t_events>
struct handles
    : public i_event_list_container
{
    handles()
    {
        declare_handle<t_context, t_events...>(static_cast<t_context*>(this), this);
    }

    virtual ~handles()
    {
        forward_delete_all_callers();
    }
};

struct master_list
{
    std::list<i_event_list*> m_event_lists;
    //std::list<i_event_list*> m_singletons_to_free;
    std::map<i_event_list*, bool> m_singletons_to_free;

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

        for(auto it=m_singletons_to_free.begin(); it!=m_singletons_to_free.end(); ++it)
            delete (it->first);
    }

//    template <typename t_event>
//    void declare()
//    {
//        //m_singletons_to_free.push_back(event_list<t_event>::auto_instance());
//    }

    template <typename t_event>
    void post(const t_event& ev)
    {
        event_list<t_event>* evlist = event_list<t_event>::auto_instance();
        evlist->add(ev);
        m_event_lists.push_back(evlist);
        m_singletons_to_free[evlist] = true;
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
