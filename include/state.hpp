
#pragma once

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
        m_context->post_transit<_new_state>();
    }

    template <typename _event>
    void post_event(_event ev)
    {
        m_context->post_event<_event>(ev);
    }

    int event_done()
    {
        return TINYSM_RESULT_BASIC_EVENT_DONE;
    }

    int forward_event()
    {
        return TINYSM_RESULT_FORWARD_EVENT;
    }



    //static bool is_context()
    //{
    //    return false;
    //}

    //int stop_processing()
    //{
    //    return TINYSM_RESULT_REQ_PROCESSOR_STOP;
    //}
};
