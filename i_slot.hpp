
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

    template <typename t_state>
    void switch_holder(bool call_entries)
    {
        if(nullptr != m_state_holder)
            delete m_state_holder;

        state_holder<t_state>* new_state_holder = new state_holder<t_state>;
        new_state_holder->m_state->m_slot = this;
        m_state_holder = new_state_holder;

        if(call_entries)
        {
            // call state on_enter
            new_state_holder->m_state->on_enter();

            // call slot(s) on_enter (only if the state has slots)

            std::cout << __PRETTY_FUNCTION__ << " t_state is_base_of i_slot_ref_list: " << std::is_base_of<sm::i_slot_ref_list, t_state>::value << std::endl;

            sfinae_call_on_enter_for_all_slots<t_state>(new_state_holder->m_state);

            // TODO: must put this in a SFINAE expression
//            if (std::is_base_of<sm::i_slot_ref_list, t_state>::value)
//            {
//                static_cast<sm::i_slot_ref_list*>(new_state_holder) ->call_on_enter_for_all_slots();
//            }
        }
    }

    template <typename t_state>
    void
    sfinae_call_on_enter_for_all_slots(typename std::enable_if<std::is_base_of<sm::i_slot_ref_list, t_state>::value, t_state>::type * st)
    {
        // t_state derives from (at least one) sm::slot, so we call each substate on_enter
        std::cout << __PRETTY_FUNCTION__ << "state has slots" << std::endl;

        st->call_on_enter_for_all_slots();
    }

    template <typename t_state>
    void
    sfinae_call_on_enter_for_all_slots(typename std::enable_if<!std::is_base_of<sm::i_slot_ref_list, t_state>::value, t_state>::type * st)
    {
        // t_state doesn't derive from any sm::slot, it doesn't have any substate
        // Do nothing.

        std::cout << __PRETTY_FUNCTION__ << "state has NO slots" << std::endl;
    }

    void forward_on_enter();
    void forward_on_exit();
};

////////////////////////////////////////////////////////////////////////////////

}

#endif // SM_I_SLOT_HPP

