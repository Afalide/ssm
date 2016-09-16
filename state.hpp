
#pragma once

template <typename t> struct context;

template <typename t_event_interface>
struct state
{
    context<t_event_interface>* m_context;

    virtual ~state(){}
};
