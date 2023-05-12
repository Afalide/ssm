
#include <ssm/state.hpp>

////////////////////////////////////////////////////////////////////////////////

node::node ()
    : name      ()
    , statep    (nullptr)
    , parent    (nullptr)
    , layer_idx (0)
    , sb        ()
    , level     (0)
    , layers    ()
    , is_active (false)
{ }

////////////////////////////////////////////////////////////////////////////////

node::layer_def&
node::layer ()
{
    if (! parent)
        SSM_ERROR ("cannot find layer of '" << name << "' which has no parent");

    return parent ->layers .at (layer_idx);
}

////////////////////////////////////////////////////////////////////////////////

void
node::delete_subtree ()
{
    // Recursively destroy all nodes below *this,
    // but don't destroy *this.

    if (! layers .size ())
        return;

    std::deque <node*> deq;

    // First add all subnodes of *this

    for (auto const& pair : layers)
        for (node* n : pair .second .possibles)
            deq .push_back (n);
    
    // Process nodes (starting with subnodes of *this)

    while (! deq .empty ())
    {
        node* n = deq .front ();
        deq .pop_front ();

        for (auto const& pair : n ->layers)
            for (node* subnode : pair .second .possibles)
                deq .push_back (subnode);

        std::cout << "deleting node '" << n ->name << "'\n";
        delete n;
    }
}

////////////////////////////////////////////////////////////////////////////////

void
node::dump ()
{
    if (! parent)
    {
        // This is most probably the main node.
        // It is not "dumpable", nor accessible anyways. Just throw.

        SSM_ERROR ("attempt to dump the main node");
    }

    std::deque <node*> deq;
    deq .push_back (this);

    std::cout << "graph {\n"
        << "node [shape=record]\n"
        << "layer_" << parent ->name << '_' << layer_idx << " [label=\"<id_" << name << "> " << name << "\"]\n";

    while (!deq .empty ())
    {
        node* n = deq .front ();
        deq .pop_front ();

        if (! n ->layers .size ())
            continue;

        for (auto layers_it = n ->layers .begin (); layers_it != n ->layers .end (); ++layers_it)
        {
            size_t lidx     = layers_it ->first;
            layer_def& ldef = layers_it ->second;

            // Push children of each layer in reverse order to deq's front, so deq's front appears in correct order
            // (we transfer from a FIFO vector to a LIFO) while preserving existing deq's back.

            for (auto possibles_it = ldef .possibles .rbegin (); possibles_it != ldef .possibles .rend (); ++possibles_it)
                deq .push_front (*possibles_it);

            // Print layer contents

            std::cout << "layer_" << n ->name << '_' << lidx << " [label=\"";

            for (auto possibles_it = ldef .possibles .begin (); possibles_it != ldef .possibles .end (); ++possibles_it)
            {
                bool first = possibles_it == ldef .possibles .begin ();
                node* possible_node = (*possibles_it);

                if (!first)
                    std::cout << '|';

                std::cout << "<id_" << possible_node ->name << "> " << possible_node ->name;
            }

            std::cout << "\"]\n";
            std::cout << "layer_" << n ->parent ->name << '_' << n ->layer_idx << ":id_" << n ->name << " -- layer_" << n ->name << '_' << lidx << '\n';
        }
    }

    std::cout << "}\n";
}

////////////////////////////////////////////////////////////////////////////////
