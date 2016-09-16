
#pragma once

#include <vector>
#include "ProtectedResource.hpp"
#include "singleton.hpp"

struct i_event
{
    i_event(){}
    virtual ~i_event(){}
    virtual int call() = 0;
};

template <typename _context, typename _event>
struct basic_event
    : public i_event
{
    _context* m_ctx;
    _event m_ev;

    basic_event(_context* ctx, _event ev) : m_ctx(ctx), m_ev(ev) {}
    virtual ~basic_event(){}

    virtual int call() override
    {
        return m_ctx->perform_current_state_handle(m_ev);
    }
};

template <typename _context, typename _new_state>
struct transit_event
    : public i_event
{
    _context* m_ctx;

    transit_event(_context* ctx) : m_ctx(ctx){}
    virtual ~transit_event(){}

    virtual int call() override
    {
        return m_ctx->perform_transit<_new_state>();
    }
};

class event_processor
    : public tmce::singleton<event_processor>
{
    typedef std::vector<i_event*> t_event_vector;
    typedef ProtectedResource<t_event_vector> t_protected_event_vector;

    t_protected_event_vector m_res;


public:

    event_processor()
        : m_res(t_event_vector())
    {
    }

    ~event_processor(){}

    void append(i_event* ev)
    {
        m_res.Lock();
        m_res.Get().push_back(ev);
        m_res.Release();
    }

    template <typename _context, typename _event>
    void post_basic_event(_context* ctx, _event ev)
    {
        i_event* ev = new basic_event<_context,_event>(ctx, ev);
        append(ev);
    }

    template <typename _context, typename _new_state>
    void post_transit_event(_context* context)
    {
        i_event* ev = new transit_event<_context,_new_state>(context);
        append(ev);
    }
};
