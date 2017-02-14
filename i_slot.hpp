
#ifndef SM_I_SLOT_HPP
#define SM_I_SLOT_HPP

#include <type_traits>
#include <iostream>
#include "state_holder.hpp"
#include "i_slot_ref_list.hpp"

namespace sm
{

////////////////////////////////////////////////////////////////////////////////

struct i_slot
    : public virtual i_slot_ref_list
{
    i_state_holder* m_state_holder;

    i_slot();
    virtual ~i_slot();

//    void on_enter_all();
//    void on_exit_all();

    void fw_islot_on_enter();
    void fw_islot_on_exit();

    template <typename t_state>
    void switch_holder_and_call_entries()
    {
        if(nullptr != m_state_holder)
        {
            m_state_holder->fw_sh_on_exit();
            delete m_state_holder;
            m_state_holder = nullptr;
        }

        set_new_holder<t_state>();
        m_state_holder->fw_sh_on_enter();
    }

    template <typename t_state>
    void set_new_holder() //(bool call_entries)
    {
//        assert(nullptr == m_state_holder); // A state hodler already exists, this
                                           // call would overwrite the existing state

//        if(nullptr != m_state_holder)
//        {
//            delete m_state_holder;
//        }

        state_holder<t_state>* new_state_holder = new state_holder<t_state>;
        new_state_holder->m_state->m_slot = this;
        m_state_holder = new_state_holder;
    }

};

////////////////////////////////////////////////////////////////////////////////

}

#endif // SM_I_SLOT_HPP

