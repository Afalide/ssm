
#pragma once

//struct istate
//{
//    istate() {}
//    virtual ~istate() {}
//};

//struct icontext;

#include <context.hpp>

template <typename _state>
struct state
{
    context<_state>* m_context;

    state() : m_context(nullptr) {}
    virtual ~state() {}

    template <typename _new_state>
    int immediate_transit()
    {
        return m_context->perform_transit<_new_state>();
    }

    template <typename _new_state>
    void post_transit()
    {
        //post transit event
    }
};
