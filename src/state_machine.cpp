
#include <ssm/state_machine.hpp>

////////////////////////////////////////////////////////////////////////////////

state_machine::found_node::found_node (node* found_, state_machine* sm_)
    : found (found_)
    , sm (sm_)
{ }

////////////////////////////////////////////////////////////////////////////////

state_machine::state_machine ()
    : hidden_node ()
    , nodes_by_id ()
{
    // We need the top-level states to be declared and entered.
    // However we don't have a "real" main state (yet) that would act as
    // the parent of sm_running and sm_stopped, so we need to declare it ourselves
    // and provide the bare-minimal infos for transitions to work.

    node* main_node = hidden_node .add_child_initial <sm_main> ("sm_main", 0);
    declare_node (get_state_id <sm_main> (), main_node);

    get_state <sm_main> ()
        .add <sm_running> ("sm_running")
        .add <sm_stopped> ("sm_stopped");

    // Top-states have been declared but are not active yet.
    // Calling transit() won't work because the nearest active parent is nullptr.
    // We need a top level ACTIVE state, which means an already working SM.
    // We don't have one yet, so we make the enter() calls here.
    // This emulate part of the transit() function (without exiting other states,
    // because we don't have any state to exit, yet).

    node* stopped_node = get_state <sm_stopped> () .found;
    enter (main_node);
    enter (stopped_node);
}

////////////////////////////////////////////////////////////////////////////////

state_machine::~state_machine ()
{
    //get_state <sm_main> () .found ->delete_subtree ();
    transit <sm_stopped> ();

    // The SM is "doomed" to stay in at least one state.
    // Regular transit() calls cannot change that.
    // So we undo what we did in the constructor: we call exit() ourselves,
    // to mimic a transit to some void-unexisting state

    node* main_node    = get_state <sm_main> () .found;
    node* stopped_node = get_state <sm_stopped> () .found;

    exit (stopped_node);
    exit (main_node);

    hidden_node .delete_subtree ();
}

////////////////////////////////////////////////////////////////////////////////

void
state_machine::declare_node (state_id_t id, node* n)
{
    if (nodes_by_id .find (id) != nodes_by_id .end ())
        SSM_ERROR ("node '" << n ->name << "' already declared, with id " << id);

    nodes_by_id [id] = n;
}

////////////////////////////////////////////////////////////////////////////////

void
state_machine::enter (node* n)
{
    if (n ->is_active)
        SSM_ERROR ("node '" << n ->name << "' is already active, cannot enter");

    n ->sb .new_state (&n ->statep);
    n ->is_active = true;
    n ->layer () .active = n;
}

////////////////////////////////////////////////////////////////////////////////

void
state_machine::recurse_enter (node* n, bool skip_active)
{
    std::deque <node*> deq;
    deq .push_back (n);

    while (! deq .empty ())
    {
        n = deq .front ();
        deq .pop_front ();

        bool const node_is_inactive = ! n ->is_active;
        bool const dont_skip        = ! skip_active;

        if (node_is_inactive && dont_skip)
            enter (n);

        if (0 == n ->layers .size ())
            continue;

        // Browse layers in reverse order (same reason as seen in node::dump)

        for (auto it = n ->layers .rbegin (); it != n ->layers .rend (); ++it)
        {
            size_t           lidx = it ->first;
            node::layer_def& ldef = it ->second;;

            if (!ldef .initial)
                SSM_ERROR ("layer " << lidx << " of node '" << n ->name << "' has no initial declared");

            deq .push_front (ldef .initial);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void
state_machine::guide_enter (std::deque <node*>& deq)
{
    if (!deq .size ())
        SSM_ERROR ("guide_enter() requires a non empty container");

    //std::cout << "guide_enter: ";
    //for (node* n : deq)
    //    std::cout << n ->name << ' ';
    //std::cout << '\n';

    // Enter each node of deq, except the last one, which will be recursively entered.
    // Also we reverse iterate, because deq is given to us as a 'PTNAP' (path to nearest active parent)
    // list, where the "outermost" node is at the back.

    for (auto it = deq .rbegin (); it != deq .rend (); ++it)
    {
        node* n = *it;
        bool last = std::next <> (it) == deq .rend ();

        if (last)
        {
            recurse_enter (n, false);
        }
        else
        {
            enter (n);
        }
    }

    // Enter all remaining unentered nodes

    recurse_enter (deq .front (), true);
}

////////////////////////////////////////////////////////////////////////////////

void
state_machine::exit (node* n)
{
    if (!n ->parent)
        SSM_ERROR ("node '" << n ->name << "' has no parent, cannot exit");

    n ->sb .delete_state (&n ->statep);
    n ->is_active = false;
    n ->layer ().active = nullptr;
}

////////////////////////////////////////////////////////////////////////////////

void
state_machine::recurse_exit (node* n)
{
    if (! n ->parent)
        SSM_ERROR ("node '" << n ->name << "' has no parent, cannot recurse exit");

    std::deque <node*> ordered_exit_deq;
    std::deque <node*> exam_deq;
    exam_deq .push_back (n);

    // Build an ordered list of nodes to exit.
    // This is the inverse operation of recurse_enter().
    // We need to use a temporary lookup list while building the final one.

    while (! exam_deq .empty ())
    {
        node* exam_node = exam_deq .front ();
        exam_deq .pop_front ();

        for (auto& pair : exam_node ->layers)
        {
            node::layer_def& ldef = pair .second;
            exam_deq .push_front (ldef .active);
        }

        ordered_exit_deq .push_front (exam_node);
    }

    for (node* node_to_exit : ordered_exit_deq)
        exit (node_to_exit);
}

////////////////////////////////////////////////////////////////////////////////

void
state_machine::transit_impl (state_id_t wanted_id)
{
    // The node we want

    auto wanted_it = nodes_by_id .find (wanted_id);

    if (nodes_by_id .end () == wanted_it)
        SSM_ERROR ("node with id " << wanted_id << " not found, cannot transit");

    node* wanted_node = wanted_it ->second;

    if (!wanted_node ->parent)
        SSM_ERROR ("node '" << wanted_node ->name << "' has no parent, cannot transit");

    // Get the Path To Nearest Active Parent

    std::deque <node*> ptnap;
    node* n = wanted_node;

    //if (wanted_node ->is_active)
    //{
    //    // We are transiting to an already active node,
    //    // meaning we must exit & re-enter the current state.
    //    // Just set a fixed
    //    
    //}

    //while (n ->layer () .active != n)
    //{
    //    ptnap .push_back (n);
    //    n = n ->parent;
    //}

    do
    {
        ptnap .push_back (n);
        n = n ->parent;
    } while (n ->layer () .active != n);

    // We need the nearest active layer, which is the layer we want to exit (recursively)
    // and then enter again with a different active state, effectively "swapping"
    // active states in that layer.

    node::layer_def& nearest_active_layer = ptnap .back () ->layer ();
    recurse_exit (nearest_active_layer .active);
    guide_enter (ptnap);
}

////////////////////////////////////////////////////////////////////////////////

void
state_machine::init ()
{
    // recurse_enter (main_node .layers [0] .initial, false);
    transit <sm_running> ();
}

////////////////////////////////////////////////////////////////////////////////

void
state_machine::deinit ()
{
    transit <sm_stopped> ();
}

////////////////////////////////////////////////////////////////////////////////
