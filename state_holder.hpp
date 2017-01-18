
#ifndef SM_STATE_HOLDER_HPP
#define SM_STATE_HOLDER_HPP

namespace sm {

////////////////////////////////////////////////////////////////////////////////

struct i_state_holder
{
    i_state_holder();
    virtual ~i_state_holder();
//    virtual void create() = 0;
//    virtual void destroy() = 0;
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
//        create();
    }

    virtual ~state_holder()
    {
//        destroy();
        delete m_state;
    }

//    virtual void create() override
//    {
//        m_state = new t_state;
//    }
//
//    virtual void destroy() override
//    {
//        delete m_state;
//    }
};

////////////////////////////////////////////////////////////////////////////////

}

#endif // SM_STATE_HOLDER_HPP

