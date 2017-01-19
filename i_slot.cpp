
#include "i_slot.hpp"
#include <cassert>

sm::i_slot::i_slot()
    : m_state_holder(nullptr)
{
}

sm::i_slot::~i_slot()
{
    if(nullptr != m_state_holder)
        delete m_state_holder;
}

void
sm::i_slot::forward_on_enter()
{
    assert(nullptr != m_state_holder);
    m_state_holder->forward_on_enter();
}

void
sm::i_slot::forward_on_exit()
{
    assert(nullptr != m_state_holder);
    m_state_holder->forward_on_exit();
}
