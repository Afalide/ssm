
#pragma once

//template <typename T> struct State;
#include "state.hpp"
#include "event_processor.hpp"

template <typename _event_interface>
struct context
{
    typedef state<_event_interface> t_state;
    t_state* m_current_state;

    context()
        : m_current_state(nullptr)
    {
    }

    virtual ~context()
    {
        if (nullptr != m_current_state)
        {
            m_current_state->m_context = nullptr;
            delete m_current_state;
        }
    }

    template <typename t_new_state>
    void perform_transit()
    {
        if (nullptr != m_current_state)
        {
            m_current_state->m_context = nullptr;
            delete m_current_state;
        }

        m_current_state = new t_new_state;
        m_current_state->m_context = this;
        //return TINYSM_RESULT_TRANSIT_EVENT_DONE;
    }
};
