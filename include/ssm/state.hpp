
#ifndef SSM_STATE_HPP
#define SSM_STATE_HPP

////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <deque>
#include <functional>
#include <map>

#include <ssm/error.hpp>

////////////////////////////////////////////////////////////////////////////////

struct state
{ };

////////////////////////////////////////////////////////////////////////////////

struct state_builder
{
    std::function <void (state**)> new_state;
    std::function <void (state**)> delete_state;
    
    state_builder ()                                = default;
    state_builder (state_builder const&)            = default;
    state_builder (state_builder&&)                 = default;
    ~state_builder ()                               = default;
    state_builder& operator= (state_builder const&) = default;
    state_builder& operator= (state_builder&&)      = default;
};

////////////////////////////////////////////////////////////////////////////////

template <typename T>
state_builder
make_state_builder ()
{
    state_builder sbuilder;

    sbuilder .new_state = [] (state** spp)
    {
        if (*spp)
            SSM_ERROR ("attempting to re-create a state, without freeing previous one");

        *spp = new T;
    };

    sbuilder .delete_state = [] (state** spp)
    {
        if (! (*spp))
            SSM_ERROR ("attempting to delete a null state");

        delete static_cast <T*> (*spp);
        *spp = nullptr;
    };

    return sbuilder;
}

////////////////////////////////////////////////////////////////////////////////

struct node
{
    struct layer_def
    {
        node* active;
        node* initial;
        std::vector <node*> possibles;

        layer_def ()
            : active (nullptr)
            , initial (nullptr)
            , possibles ()
        { }
    };

    std::string                  name;
    state*                       statep;
    node*                        parent;
    size_t                       layer_idx;
    state_builder                sb;
    size_t                       level;
    std::map <size_t, layer_def> layers;
    bool                         is_active;

    node ();
    layer_def& layer ();
    void delete_subtree ();
    void dump ();

    template <typename T>
    node* add_child (std::string const& name_, size_t layer_idx_)
    {
        node* n = new node;

        n ->name = name_;
        n ->parent = this;
        n ->layer_idx = layer_idx_;
        n ->level = level + 1;
        n ->sb = make_state_builder <T> ();

        layers [layer_idx_] .possibles .push_back (n);

        return n;
    }

    template <typename T>
    node* add_child_initial (std::string const& name_, size_t layer_idx_)
    {
        auto it = layers .find (layer_idx_);

        if (it != layers .end ())
            if (it ->second .initial)
                SSM_ERROR ("layer " << it ->first << " of node '" << this ->name << "' already has an initial child declared, cannot add '" << name_ << '\'');

        node* n = add_child <T> (name_, layer_idx_);
        layers [layer_idx_] .initial = n;
        return n;
    }

};

////////////////////////////////////////////////////////////////////////////////

#endif
