
#ifndef STATE_HPP
#define STATE_HPP

#include <type_traits>
#include <cassert>

#include "slot.hpp"

//struct null_state
//{
//    void on_enter(){}
//    void on_exit(){}
//};

////////////////////////////////////////////////////////////////////////////////

namespace sm
{

////////////////////////////////////////////////////////////////////////////////

struct no_parent{};

////////////////////////////////////////////////////////////////////////////////

//template <typename t_crt>
//struct parent_state
//{
//    typedef t_crt t_parent;
//}

////////////////////////////////////////////////////////////////////////////////

template <typename t_crt, typename t_parent=no_parent>
struct basic_state
{
    typedef t_parent parent;

    i_slot* m_slot;

    basic_state()
        : m_slot(nullptr)
    {
    }

    virtual ~basic_state()
    {
    }

    void assign_slot(i_slot* slt)
    {
        m_slot = slt;
    }

    template <typename t_state>
    void transit()
    {
        // static assert ( (  t_state parent declared && this parent declared && parents are same ) xor ( no parent declared on this ) )

        static_assert(std::is_same<parent, typename t_state::parent>::value, "You can only transit between states with the same parent");

        if(nullptr == m_slot)
        {
            std::cerr << "error, this state doesn't have any declared slot" << std::endl;
        }

        assert(nullptr != m_slot); // Make sure you have assigned a slot to this state (this state must be aware of it's owner slot).
//        m_slot->fw_islot_on_exit();
        m_slot->switch_holder_and_call_entries<t_state>();
//        m_slot->fw_islot_on_enter();

        // This object is now deleted: make sure you don't use it anymore
    }
};

////////////////////////////////////////////////////////////////////////////////

}

////////////////////////////////////////////////////////////////////////////////

/*
template <typename t_state>
t_state* enter_in()
{
    t_state* st = new t_state;
    st-> template enter_hierarchy<t_state>(st);
    return st;
}

template <typename t_state>
void exit_from(t_state* st)
{
    st-> template exit_hierarchy<t_state>(st);
    delete st;
}


template <typename t_crt, typename t_child_state=null_state, typename t_parent_state=null_state>
struct state
{
    typedef t_child_state   child_state_t;
    typedef t_parent_state  parent_state_t;

    child_state_t*   m_child_state;
    parent_state_t*  m_parent_state;

    state()
    {
//        std::cout << __PRETTY_FUNCTION__ << std::endl;
//        static_cast<t_crt*>(this)->on_enter();
        m_child_state = new child_state_t;
    }

    virtual ~state()
    {
        delete m_child_state;
//        static_cast<t_crt*>(this)->on_exit();
//        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    // State objects are instantiated from outermost child to outermost parent.
    // This behavior is the opposite of what we want, so we use this
    // impl trick to call the on_enter() functions from outermost parent
    // to outermost child, and the on_exit() functions in the exact
    // reverse order.

//    template <typename T = t_crt, typename std::enable_if<!std::is_same<null_state,T>::value,T>::type>
    template <typename T>
    void enter_hierarchy(typename std::enable_if<!std::is_same<typename T::child_state_t,null_state>::value,T>::type *)
    {
//        std::cout << __FUNCTION__ << ": child concrete state" << std::endl;
        static_cast<t_crt*>(this)->on_enter();
        m_child_state->enter_hierarchy<>(m_child_state);
    }

//    template <typename T = t_crt, typename ...>
    template <typename T>
    void enter_hierarchy(T*)
    {
//        std::cout << __FUNCTION__ << ": child unknown or null state" << std::endl;
        static_cast<t_crt*>(this)->on_enter();
    }

    template <typename T>
    void exit_hierarchy(typename std::enable_if<!std::is_same<typename T::child_state_t,null_state>::value,T>::type *)
    {
//        std::cout << __FUNCTION__ << ": child concrete state" << std::endl;
        m_child_state->exit_hierarchy<>(m_child_state);
        static_cast<t_crt*>(this)->on_exit();
    }

//    template <typename T = t_crt, typename ...>
    template <typename T>
    void exit_hierarchy(T*)
    {
//        std::cout << __FUNCTION__ << ": child unknown or null state" << std::endl;
        static_cast<t_crt*>(this)->on_exit();
    }

    template <typename t_state>
    void transit_child()
    {
        if(nullptr != m_child_state)
            delete m_child_state;

        m_child_state = new t_state;
        m_child_state->m_parent_state = this;
    }

    template <typename t_state>
    void transit()
    {
        static_assert(std::is_same<typename t_state::parent_state_t, parent_state_t>::value, "You can only transit between states with the same parent");
        static_assert(! std::is_same<parent_state_t, null_state>::value, "You cannot perform a transition with a null_state as parent");

//        parent.switch_to_children<newstate>()
        m_parent_state->transit_child<t_state>();
    }
};
*/

#endif // STATE_HPP
