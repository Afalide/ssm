
#ifndef SM_STATE_HOLDER_HPP
#define SM_STATE_HOLDER_HPP

#include "i_slot_ref_list.hpp"

namespace sm {

////////////////////////////////////////////////////////////////////////////////

struct i_state_holder
{
    i_state_holder();
    virtual ~i_state_holder();
    virtual void forward_on_enter() = 0;
    virtual void forward_on_exit() = 0;

    virtual void forward_on_enter_on_slots() = 0;
    virtual void forward_on_exit_on_slots() = 0;
};

////////////////////////////////////////////////////////////////////////////////

template <typename t_state>
struct state_holder
    : public i_state_holder
{
    t_state* m_state;

    state_holder()
        : m_state(new t_state)
    {
    }

    virtual ~state_holder()
    {
        delete m_state;
    }

    virtual void forward_on_enter() override
    {
        m_state->on_enter();
    }

    virtual void forward_on_exit() override
    {
        m_state->on_exit();
    }

    virtual void forward_on_enter_on_slots() override
    {
        sfinae_call_on_enter_for_all_slots<t_state>(m_state);
    }

    virtual void forward_on_exit_on_slots() override
    {
        sfinae_call_on_exit_for_all_slots<t_state>(m_state);
    }

    template <typename _t_state>
    void sfinae_call_on_enter_for_all_slots(typename std::enable_if<std::is_base_of<sm::i_slot_ref_list, _t_state>::value, _t_state>::type * st)
    {
        // t_state derives from (at least one) sm::slot, so we call each substate's on_enter
        st->call_on_enter_for_all_slots();
    }

    template <typename _t_state>
    void sfinae_call_on_enter_for_all_slots(typename std::enable_if<!std::is_base_of<sm::i_slot_ref_list, _t_state>::value, _t_state>::type * st)
    {
        // t_state doesn't derive from any sm::slot, it doesn't have any substate
        // Do nothing.
    }

    template <typename _t_state>
    void sfinae_call_on_exit_for_all_slots(typename std::enable_if<std::is_base_of<sm::i_slot_ref_list, _t_state>::value, _t_state>::type * st)
    {
        // t_state derives from (at least one) sm::slot, so we call each substate's on_exit
        st->call_on_exit_for_all_slots();
    }

    template <typename _t_state>
    void sfinae_call_on_exit_for_all_slots(typename std::enable_if<!std::is_base_of<sm::i_slot_ref_list, _t_state>::value, _t_state>::type * st)
    {
        // t_state doesn't derive from any sm::slot, it doesn't have any substate
        // Do nothing.
    }
};

////////////////////////////////////////////////////////////////////////////////

}

#endif // SM_STATE_HOLDER_HPP

