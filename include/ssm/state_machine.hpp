
#ifndef SSM_STATE_MACHINE_HPP
#define SSM_STATE_MACHINE_HPP

////////////////////////////////////////////////////////////////////////////////

#include <ssm/state.hpp>
#include <ssm/progid.hpp>

////////////////////////////////////////////////////////////////////////////////

/*
                            ┌───────┐
                            │sm_main│
                            └────┬──┘            sm_main and sm_exit are managed
                                 │               by the state_machine class.
                       ┌─────────┴──────────┐
 state_A is the        │                    │
 "root" state      ┌───┴───┐            ┌───┴───┐
                   │state_A│            │sm_exit│
                   └───┬───┘            └───────┘
                       │
                ┌──────┴──────────┐
                │                 │
            ┌───┴───┐    ┌────────┴────────┐
            │state_B│    │state_C / state_D│
            └───┬───┘    └─────────────────┘
                │
              ┌─┴─┐
              │...│       ... more user states here
              └───┘
*/

struct state_machine
{
    node hidden_node;
    std::map <state_id_t, node*> nodes_by_id;

    struct sm_main    : public state { };
    struct sm_running : public state { };
    struct sm_stopped : public state { };

    struct found_node
    {
        node*          found;
        state_machine* sm;

        found_node (node* found_, state_machine* sm_);

        found_node (found_node const&)            = default;
        found_node (found_node&&)                 = default;
        found_node& operator= (found_node const&) = default;
        found_node& operator= (found_node&&)      = default;

        template <typename T>
        found_node&
        add_initial (std::string const& name, size_t layer_idx = 0)
        {
            node* child = found ->add_child_initial <T> (name, layer_idx);
            sm ->declare_node (get_state_id <T> (), child);
            return *this;
        }

        template <typename T>
        found_node&
        add (std::string const& name, size_t layer_idx = 0)
        {
            node* child = found ->add_child <T> (name, layer_idx);
            sm ->declare_node (get_state_id <T> (), child);
            return *this;
        }
    };

    state_machine ();
    ~state_machine ();

    void declare_node (state_id_t id, node* n);

    template <typename T>
    found_node get_state ()
    {
        state_id_t id = get_state_id <T> ();

        auto it = nodes_by_id .find (id);

        if (it == nodes_by_id .end ())
            SSM_ERROR ("state with id " << id << " not found");

        return found_node {it ->second, this};
    }

    template <typename T>
    found_node add_root_state (std::string const& name)
    {
        get_state <sm_running> () .add_initial <T> (name);

        // We must return the newly added found_node instead of the parent.
        // This "hides" the main_node, and allows the user to chain with add_xxx calls.

        return get_state <T> ();
    }

    void enter (node* n);

    void recurse_enter (node* n, bool skip_active);

    void guide_enter (std::deque <node*>& deq);

    void exit (node* n);

    void recurse_exit (node* n);
    
    void transit_impl (state_id_t wanted_id);

    template <typename T>
    void transit ()
    {
        transit_impl (get_state_id <T> ());
    }

    void init ();

    void deinit ();
};

#endif
