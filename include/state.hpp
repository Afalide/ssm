
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

    int event_done()
    {
        return TINYSM_RESULT_BASIC_EVENT_DONE;
    }

    //int stop_processing()
    //{
    //    return TINYSM_RESULT_REQ_PROCESSOR_STOP;
    //}
};
