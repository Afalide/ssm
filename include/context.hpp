
#pragma once

#include <cassert>
#include <has_handle.hpp>

//struct icontext
//{
//    icontext(){}
//    virtual ~icontext(){}
//};

//struct state;

template <typename _state>
struct context
{
    //static assert _state derives from state

    _state* m_current_state;

    context() : m_current_state (nullptr) {}

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
        return 123;
    }

    template <typename _event>
    int current_state_handle (_event ev)
    {
        assert(nullptr != m_current_state);
        return m_current_state->handle(ev);
    }
};
