
#ifndef SM_I_SLOT_HPP
#define SM_I_SLOT_HPP

#include "state_holder.hpp"

namespace sm
{

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

    void forward_on_enter();
    void forward_on_exit();
};

////////////////////////////////////////////////////////////////////////////////

}

#endif // SM_I_SLOT_HPP

