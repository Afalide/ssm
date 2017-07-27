
#include <iostream>
#include <cassert>
#include <type_traits>
#include <limits>

////////////////////////////////////////////////////////////////////////////////

// Forward declarations

struct i_state_dtor;
struct i_slot;

template <typename t_state, typename...t_states>
struct create_state_impl;

////////////////////////////////////////////////////////////////////////////////

struct i_state
{
    i_slot* m_owner;
	i_state_dtor* m_dtor;

    i_state()
        : m_owner (nullptr)
		, m_dtor (nullptr)
    { }
    virtual ~i_state(){}
    i_state(const i_state& other) = delete;
    i_state& operator=(const i_state& other) = delete;
};

////////////////////////////////////////////////////////////////////////////////

struct i_slot
{
    i_state** m_states;

    i_slot()
        : m_states(nullptr)
    {}
    virtual ~i_slot(){}
    i_slot(const i_slot&) = delete;
    i_slot& operator=(const i_slot&) = delete;

    virtual void create_all_slots() = 0;
    virtual void delete_all_slots() = 0;



    template <typename t_old_state, typename t_new_state>
    void switch_state(unsigned int idx)
    {
        // Delete the old state

		t_old_state* old_state = dynamic_cast<t_old_state*>(m_states[idx]);
		assert(nullptr != old_state);
		old_state->m_dtor->perform_delete();

        // Replace it with the new one

		create_state_impl<t_new_state>::impl(idx, this);
    }
};

////////////////////////////////////////////////////////////////////////////////

// SFINAE functions to create and delete a state's children
// if and only if the given state has children

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

////////////////////////////////////////////////////////////////////////////////

// MPL function to create a single state or a list of states
// This will invoke the SFINAE function above to determine if each of the
// create state also requires to instantiate it's children

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

////////////////////////////////////////////////////////////////////////////////

// Deleting a state uses a different procedure because of transitions.
// When deleting a state, one of it's child may have changed. Using an i_state
// reference is not sufficient, because each child state may also have children,
// therefore the SFINAE procedure for children deletion needs to be invoked on
// the state to delete, which requires the true derived type of said state to be known.
// This method uses a functor which can be carried around in an opaque type.
// Invoking this functor will delete the given state and invalidate the functor
// object (delete *this).

struct i_state_dtor
{
	virtual ~i_state_dtor(){}
	virtual void perform_delete() = 0;
};

template <typename t_state>
struct state_dtor : public i_state_dtor
{
    t_state* m_state_to_delete;

    state_dtor(t_state* st)
        : m_state_to_delete(st)
    { }

    virtual ~state_dtor( )
    {
        m_state_to_delete = nullptr;
    }

    state_dtor(const state_dtor&) = delete;
    state_dtor& operator=(const state_dtor&) = delete;

	virtual void perform_delete() override
	{
        // Be safe, check first

		//t_state* true_type_state = dynamic_cast<t_state*>(m_state_to_delete);
		//assert(nullptr != true_type_state);
        assert(nullptr != m_state_to_delete);

		//t_state* true_type_state = static_cast<t_state*>(st);  // Cheaper, use this instead of
                                                                 // dynamic casts in release config

		// Delete the state's children if required

		sfinae_delete_all_slots_of<t_state>(m_state_to_delete);

		// Keep the owner's infos

		i_slot* owner = m_state_to_delete->m_owner;
		unsigned int idx = m_state_to_delete->m_slot_pos;

		// Update the owner

		owner->m_states[idx] = nullptr;

		// Delete the state

		delete m_state_to_delete;

		// Delete ourselves

		delete this;
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
        m_states = new i_state* [sizeof...(t_states)];
        create_state_impl<t_states...>::impl(0, this);
    }

    virtual void delete_all_slots() override
    {
//        std::cout << __PRETTY_FUNCTION__<< std::endl;
        //delete_all_slots_extract<0, t_states...>(this);
        //delete_state_impl<t_states...>::impl(0, this);

		for (unsigned int i = 0; i < sizeof...(t_states); ++i)
		{
			m_states[i]->m_dtor->perform_delete();
			m_states[i] = nullptr;
		}

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
		m_dtor = new state_dtor<t_crt>(static_cast<t_crt*>(this));
    }

    virtual ~state()
    {
		//delete m_dtor;  // The dtor object will delete itself upon execution
    }

    template <typename t_new_state>
    void go()
    {
        assert(nullptr != m_owner);
		m_owner->switch_state<t_crt, t_new_state>(m_slot_pos);

		//delete_state_impl<t_crt>::impl(m_slot_pos, m_owner);
		//create_state_impl<t_new_state>::impl(m_slot_pos, m_owner);
    }
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

struct state_d : public state<state_d>
{
	state_d() { std::cout << "enter " << __FUNCTION__ << std::endl; }
	virtual ~state_d() { std::cout << "exit " << __FUNCTION__ << std::endl; }
};

int main()
{
    std::cout << "main begin" << std::endl;

    state_machine<boss> sm;

	boss* state_boss = dynamic_cast<boss*> (sm.m_states[0]);
	state_b* sb = dynamic_cast<state_b*>(state_boss->m_states[1]);

	sb->go<state_d>();

    std::cout << "main end" << std::endl;
    return 0;
}
