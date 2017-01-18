
#ifndef SM_SLOT_HPP
#define SM_SLOT_HPP

#include <iostream>
#include "state_holder.hpp"

namespace sm {

////////////////////////////////////////////////////////////////////////////////

struct i_slot
{
    i_state_holder* m_state_holder;

    i_slot();

    virtual ~i_slot();

    template <typename t_state>
    void switch_holder()
    {
        if(nullptr != m_state_holder)
            delete m_state_holder;

        m_state_holder = new state_holder<t_state>;
    }
};

////////////////////////////////////////////////////////////////////////////////

template <typename t_start_state>
struct slot
    : public i_slot
{
    slot()
    {
        switch_holder<t_start_state>();
    }

    virtual ~slot()
    {
    }
};

////////////////////////////////////////////////////////////////////////////////

}

#endif // SM_SLOT_HPP
