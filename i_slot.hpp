
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

    void forward_on_enter();
    void forward_on_exit();

    template <typename t_state>
    void switch_holder(bool call_entries)
    {
        if(nullptr != m_state_holder)
        {
            if(call_entries)
            {
                // call slots' on_exit (only if this state has slots)
                m_state_holder->forward_on_exit_on_slots();

                // call state's on_enter
                m_state_holder->forward_on_exit();
            }

            delete m_state_holder;
        }

        state_holder<t_state>* new_state_holder = new state_holder<t_state>;
        new_state_holder->m_state->m_slot = this;
        m_state_holder = new_state_holder;

        if(call_entries)
        {
            // call state's on_enter

//            new_state_holder->m_state->on_enter();
            m_state_holder->forward_on_enter();

            // call slot's on_enter (only if the state has slots)

            m_state_holder->forward_on_enter_on_slots();

//            std::cout << __PRETTY_FUNCTION__ << " t_state is_base_of i_slot_ref_list: " << std::is_base_of<sm::i_slot_ref_list, t_state>::value << std::endl;
//            sfinae_call_on_enter_for_all_slots<t_state>(new_state_holder->m_state);
        }
    }

//    template <typename t_state>
//    void sfinae_call_on_enter_for_all_slots(typename std::enable_if<std::is_base_of<sm::i_slot_ref_list, t_state>::value, t_state>::type * st)
//    {
//        // t_state derives from (at least one) sm::slot, so we call each substate's on_enter
//
////        std::cout << __PRETTY_FUNCTION__ << "state has slots" << std::endl;
//        st->call_on_enter_for_all_slots();
//    }

//    template <typename t_state>
//    void sfinae_call_on_enter_for_all_slots(typename std::enable_if<!std::is_base_of<sm::i_slot_ref_list, t_state>::value, t_state>::type * st)
//    {
//        // t_state doesn't derive from any sm::slot, it doesn't have any substate
//        // Do nothing.
//
////        std::cout << __PRETTY_FUNCTION__ << "state has NO slots" << std::endl;
//    }

//    template <typename t_state>
//    void
//    sfinae_call_on_exit_for_all_slots(typename std::enable_if<std::is_base_of<sm::i_slot_ref_list, t_state>::value, t_state>::type * st)
//    {
//        // t_state derives from (at least one) sm::slot, so we call each substate's on_exit
//
////        std::cout << __PRETTY_FUNCTION__ << "state has slots" << std::endl;
//        st->call_on_exit_for_all_slots();
//    }
//
//    template <typename t_state>
//    void
//    sfinae_call_on_exit_for_all_slots(typename std::enable_if<!std::is_base_of<sm::i_slot_ref_list, t_state>::value, t_state>::type * st)
//    {
//        // t_state doesn't derive from any sm::slot, it doesn't have any substate
//        // Do nothing.
//
////        std::cout << __PRETTY_FUNCTION__ << "state has NO slots" << std::endl;
//    }


};

////////////////////////////////////////////////////////////////////////////////

}

#endif // SM_I_SLOT_HPP

