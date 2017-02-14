
#include "i_slot.hpp"
#include <cassert>

sm::i_slot::i_slot()
    : m_state_holder(nullptr)
{
    add_slot_ref(this);
}

sm::i_slot::~i_slot()
{
    if(nullptr != m_state_holder)
        delete m_state_holder;
}

void
sm::i_slot::fw_islot_on_enter()
{
    m_state_holder->fw_sh_on_enter();
}

void
sm::i_slot::fw_islot_on_exit()
{
    m_state_holder->fw_sh_on_exit();
}


//void
//sm::i_slot::on_enter_all()
//{
//    m_state_holder->on_enter_all();
//}

//void
//sm::i_slot::forward_on_enter()
//{
//    assert(nullptr != m_state_holder);
//    m_state_holder->forward_on_enter();
//}
//
//void
//sm::i_slot::forward_on_exit()
//{
//    assert(nullptr != m_state_holder);
//    m_state_holder->forward_on_exit();
//}
