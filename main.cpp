
#include <iostream>
#include <cassert>
#include <type_traits>
#include <limits>

struct i_slot;

struct i_state
{
    i_slot* m_owner;
    virtual ~i_state(){}
};

struct i_slot
{
    i_state** m_states;

    virtual ~i_slot(){}
    virtual void create_all_slots() = 0;
    virtual void delete_all_slots() = 0;

//    template <typename t_old_state, typename t_new_state>
//    void switch_state(unsigned int idx)
//    {
//        // Delete the old state
//
//        t_old_state* old_state = static_cast<t_old_state*> (m_states[idx]);
//        sfinae_delete_all_slots_of<t_old_state>(old_state);
//        delete old_state;
//        m_states[idx] = nullptr;
//
//        // Replace it with the new one
//
//        t_new_state* new_state = new t_new_state;
//        sfinae_create_all_slots_of<t_new_state>(new_state);
//        m_states[idx] = new_state;
//        new_state->m_owner = this;
//        new_state->
//    }
};

// SFINAE functions to create and delete a state's children

template <typename _t_state>
void sfinae_create_all_slots_of(typename std::enable_if<std::is_base_of<i_slot, _t_state>::value, _t_state>::type* st)
{
    st->create_all_slots();
}

template <typename _t_state>
inline void sfinae_create_all_slots_of(typename std::enable_if<!std::is_base_of<i_slot, _t_state>::value, _t_state>::type*)
{
}

template <typename _t_state>
void sfinae_delete_all_slots_of(typename std::enable_if<std::is_base_of<i_slot, _t_state>::value, _t_state>::type* st)
{
    st->delete_all_slots();
}

template <typename _t_state>
inline void sfinae_delete_all_slots_of(typename std::enable_if<!std::is_base_of<i_slot, _t_state>::value, _t_state>::type*)
{
}

// Variadic functions to create all the children (slots) of a state

//template <unsigned int idx, typename t_state>
//void create_slot_impl(i_slot* parent_slot)
//{
////    std::cout << __PRETTY_FUNCTION__<< " - creating state at slot " << idx <<  std::endl;
//
//    // Create the new state at the specified slot index
//
//    t_state* new_state = new t_state;
//    new_state->m_slot_pos = idx;
//    new_state->m_owner = parent_slot;
//    parent_slot->m_states[idx] = new_state;
//
//    // If the new state has children, recursively create them
//
//    sfinae_create_all_slots_of<t_state>(new_state);
//}
//
//template <unsigned int idx, typename t_state, typename...t_states>
//void create_all_slots_extract(i_slot* parent_slot)
//{
//    create_slot_impl<idx, t_state>(parent_slot);
//    create_all_slots_extract<idx + 1, t_states...>(parent_slot);
//}
//
//template <unsigned int idx>
//void create_all_slots_extract(i_slot*)
//{
//}

// Routing works as follows:
//   impl <A, {B,C,D}>    // user writes this (which calls a router)
//     impl <A>           // the router calls the impl
//     impl <B, {C,D}>    // the router calls another router
//       impl <B>         // the router calls the impl
//       impl <C, {D}>    // the router calls another router
//         impl <C>       // the router calls the impl
//         impl <D>       // the router calls the impl again (pack is empty, this resolves to the single arg call)
// Note that if you write this:
// impl <A>
// The implementation will be called directly, no routing involved (otherwises, we would get some recursive infinite loop)

// Base template, which acts as the router.
// The router calls the implementation
// After that, it calls the router again, or direct implementation if the arg pack is empty

template <typename t_state, typename...t_states>
struct create_state_impl
{
    static void impl(unsigned int idx, i_slot* owner)
    {
        //std::cout << "router calls impl" << std::endl;
        create_state_impl<t_state>::impl(idx, owner);
        //std::cout << "router calls next (maybe router or direct impl)" << std::endl;
        create_state_impl<t_states...>::impl(idx + 1, owner);
    }
};

// Specialization, which acts as the implementation

template <typename t_state>
struct create_state_impl<t_state>
{
    static void impl(unsigned int idx, i_slot* owner)
    {
        //std::cout << "impl idx " << idx << std::endl;

        // Create the new state at the specified slot index

        t_state* new_state = new t_state;
        new_state->m_slot_pos = idx;
        new_state->m_owner = owner;
        owner->m_states[idx] = new_state;

        // If the new state has children, recursively create them

        sfinae_create_all_slots_of<t_state>(new_state);
    }
};

// Variadic functions to delete all the children of a state

//template <unsigned int idx, typename t_state>
//void delete_slot_impl(i_slot* parent_slot)
//{
//    t_state* st = static_cast<t_state*> (parent_slot->m_states[idx]);
//
//    // If the state has slots, delete them
//
//    sfinae_delete_all_slots_of<t_state>(st);
//
//    // Delete the state
//
//    delete st;
//    parent_slot->m_states[idx] = nullptr;
//}
//
//template <unsigned int idx, typename t_state, typename...t_states>
//void delete_all_slots_extract(i_slot* parent_slot)
//{
//    delete_slot_impl<idx, t_state>(parent_slot);
//    delete_all_slots_extract<idx + 1, t_states...>(parent_slot);
//}
//
//template <unsigned int idx>
//void delete_all_slots_extract(i_slot*)
//{
//}

template <typename t_state, typename...t_states>
struct delete_state_impl
{
    static void impl(unsigned int idx, i_slot* owner)
    {
        //std::cout << "router calls impl" << std::endl;
        delete_state_impl<t_state>::impl(idx, owner);
        //std::cout << "router calls next (maybe router or direct impl)" << std::endl;
        delete_state_impl<t_states...>::impl(idx + 1, owner);
    }
};

// Specialization, which acts as the implementation

template <typename t_state>
struct delete_state_impl<t_state>
{
    static void impl(unsigned int idx, i_slot* owner)
    {
        t_state* st = static_cast<t_state*> (owner->m_states[idx]);

        // If the state has slots, delete them

        sfinae_delete_all_slots_of<t_state>(st);

        // Delete the state

        delete st;
        parent_slot->m_states[idx] = nullptr;
    }
};

// Slot class

template <typename...t_states>
struct slot
    : public i_slot
{
    slot()
    {
        m_states = nullptr;
    }

    virtual ~slot()
    {
    }

    virtual void create_all_slots() override
    {
//        std::cout << __PRETTY_FUNCTION__<< std::endl;
        m_states = new i_state* [sizeof...(t_states)];
//        create_all_slots_extract<t_states...>(0, this);
        create_state_impl<t_states...>::impl(0, this);
    }

    virtual void delete_all_slots() override
    {
//        std::cout << __PRETTY_FUNCTION__<< std::endl;
        delete_all_slots_extract<0, t_states...>(this);
        delete[] m_states;
        m_states = nullptr;
    }
};

// State class

template <typename t_crt>
struct state : public i_state
{
    unsigned int m_slot_pos;

    state()
        : m_slot_pos(std::numeric_limits<unsigned int>::max())
    {
    }

    virtual ~state()
    {
    }

//    template <typename t_new_state>
//    void go()
//    {
//        assert(nullptr != m_owner);
//        m_owner->
//    }
};

// Main SM class
// Helper which runs a starting state, and requests
// that state to exit upon destruction

template <typename t_state>
struct state_machine
    : public slot<t_state>
{
    state_machine()
    {
        slot<t_state>::create_all_slots();
    }

    ~state_machine()
    {
        slot<t_state>::delete_all_slots();
    }
};


////////////////////////////////////////////////////////////////////
// User space
////////////////////////////////////////////////////////////////////

struct state_a;
struct state_b;
struct state_b1;
struct state_b2;
struct state_c;

struct boss
    : public state<boss>
    , public slot<state_a, state_b, state_c>
{
    boss(){std::cout << "enter " << __FUNCTION__ << std::endl;}
    virtual ~boss(){std::cout << "exit " << __FUNCTION__ << std::endl;}
};

struct state_a : public state<state_a>
{
    state_a(){std::cout << "enter " << __FUNCTION__ << std::endl;}
    virtual ~state_a(){std::cout << "exit " << __FUNCTION__ << std::endl;}
};

struct state_b : public state<state_b>
               , public slot<state_b1, state_b2>
{
    state_b(){std::cout << "enter " << __FUNCTION__ << std::endl;}
    virtual ~state_b(){std::cout << "exit " << __FUNCTION__ << std::endl;}
};

struct state_b1 : public state<state_b1>
{
    state_b1(){std::cout << "enter " << __FUNCTION__ << std::endl;}
    virtual ~state_b1(){std::cout << "exit " << __FUNCTION__ << std::endl;}
};

struct state_b2 : public state<state_b2>
{
    state_b2(){std::cout << "enter " << __FUNCTION__ << std::endl;}
    virtual ~state_b2(){std::cout << "exit " << __FUNCTION__ << std::endl;}
};

struct state_c : public state<state_c>
{
    state_c(){std::cout << "enter " << __FUNCTION__ << std::endl;}
    virtual ~state_c(){std::cout << "exit " << __FUNCTION__ << std::endl;}
};

int main()
{
    std::cout << "main begin" << std::endl;

    state_machine<boss> sm;

    std::cout << "main end" << std::endl;
    return 0;
}
