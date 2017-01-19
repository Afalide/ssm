
#ifndef SM_STATE_HOLDER_HPP
#define SM_STATE_HOLDER_HPP

namespace sm {

////////////////////////////////////////////////////////////////////////////////

struct i_state_holder
{
    i_state_holder();
    virtual ~i_state_holder();
    virtual void forward_on_enter() = 0;
    virtual void forward_on_exit() = 0;
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
};

////////////////////////////////////////////////////////////////////////////////

}

#endif // SM_STATE_HOLDER_HPP

