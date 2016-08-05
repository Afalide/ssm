
#pragma once

#include <cassert>
//#include <has_handle.hpp>
//#include <event_result.hpp>
#include <event_processor.hpp>

template <typename _state>
struct context
{
    //static assert _state derives from state

    typedef context<_state> this_type;

    _state* m_current_state;
    event_processor* m_event_processor;

    context() : m_current_state (nullptr), m_event_processor(nullptr) {}

    virtual ~context() 
    {
        if (nullptr != m_current_state)
        {
            m_current_state->m_context = nullptr;
            delete m_current_state;
        }
    }

    template <typename _new_state>
    int perform_transit()
    {
        if (nullptr != m_current_state)
        {
            m_current_state->m_context = nullptr;
            delete m_current_state;
        }

        m_current_state = new _new_state;
        m_current_state->m_context = this;
        return TINYSM_RESULT_TRANSIT_EVENT_DONE;
    }

    template <typename _new_state>
    void post_transit()
    {
        assert(nullptr != m_event_processor);
        m_event_processor->post_transit_event<this_type, _new_state>(this);
    }

    template <typename _event>
    int perform_current_state_handle (_event ev)
    {
        assert(nullptr != m_current_state);
        return m_current_state->handle(ev);
    }

    template <typename _event>
    void post_event(_event ev)
    {
        assert(nullptr != m_event_processor);
        m_event_processor->post_basic_event<this_type, _event>(this, ev);
    }
};
