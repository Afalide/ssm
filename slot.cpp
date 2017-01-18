
#include "slot.hpp"



sm::i_slot::i_slot()
    : m_state_holder(nullptr)
{
}

sm::i_slot::~i_slot()
{
    if(nullptr != m_state_holder)
        delete m_state_holder;
}


