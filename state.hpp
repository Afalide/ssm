
#ifndef STATE_HPP
#define STATE_HPP

#include <type_traits>

struct null_state
{
};


namespace sm
{

////////////////////////////////////////////////////////////////////////////////

struct i_resource
{
    virtual ~i_resource(){}
//    virtual void create() = 0;
//    virtual void destroy() = 0;
};

////////////////////////////////////////////////////////////////////////////////

template <typename t_>
struct resource
    : public i_resource
{
    t_* m_res;

    resource()
        : m_res(new t_)
    {
    }

    virtual ~resource()
    {
        delete m_res;
    }

//    virtual void create() override
//    {
//        m_res = new t_;
//    }
//
//    virtual void destroy() override
//    {
//        delete m_res;
//    }
};

////////////////////////////////////////////////////////////////////////////////

template <typename t_crt>
struct basic_state
{

};

////////////////////////////////////////////////////////////////////////////////

template <typename t_state>
struct state_holder
{
    t_state* m_state;

    state_holder()
        : m_state(nullptr)
    {}

    virtual ~state_holder(){}

    template <typename t_child_state>
    void make_transit()
    {

    }
};

////////////////////////////////////////////////////////////////////////////////

template <typename t_child>
struct has_child
{
//    t_child* m_child_state;
    i_resource* m_state_res;

    has_child()
        : m_state_res(nullptr)
    {
        transit_child<t_child>();
    }

    virtual ~has_child()
    {
        delete m_state_res;
    }

    template <typename t_state>
    void transit_child()
    {
        if(nullptr != m_state_res)
            delete m_state_res;

        m_state_res = new resource<t_state>;
    }
};

////////////////////////////////////////////////////////////////////////////////

}

////////////////////////////////////////////////////////////////////////////////





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

    template <typename t_state/*, typename std::enable_if<!std::is_same<typename t_state::child_state_t, null_state>::value,t_state>::type*/>
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

#endif // STATE_HPP
