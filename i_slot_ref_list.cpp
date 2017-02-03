
#include "i_slot_ref_list.hpp"
#include "i_slot.hpp"

sm::i_slot_ref_list::i_slot_ref_list()
    : m_slot_refs()
{
//    std::cout << "ctor i_slot_ref_list" << std::endl;
}

sm::i_slot_ref_list::~i_slot_ref_list()
{
//    std::cout << "dtor i_slot_ref_list" << std::endl;
}

void
sm::i_slot_ref_list::add_slot_ref(i_slot* s)
{
    m_slot_refs.push_back(s);
}

void
sm::i_slot_ref_list::call_on_enter_for_all_slots()
{
    for(auto it = m_slot_refs.begin(); it != m_slot_refs.end(); ++it)
    {
        i_slot* slot = *it;
        slot->forward_on_enter();
    }
}

void
sm::i_slot_ref_list::call_on_exit_for_all_slots()
{
    for(auto it = m_slot_refs.rbegin(); it != m_slot_refs.rend(); ++it)
    {
        i_slot* slot = *it;
        slot->forward_on_exit();
    }
}
