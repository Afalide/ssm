
#ifndef SM_I_SLOT_REF_LIST_HPP
#define SM_I_SLOT_REF_LIST_HPP

#include <vector>

namespace sm
{

////////////////////////////////////////////////////////////////////////////////

struct i_slot;

////////////////////////////////////////////////////////////////////////////////

struct i_slot_ref_list
{
    std::vector<i_slot*> m_slot_refs;

    i_slot_ref_list();
    virtual ~i_slot_ref_list();

    void add_slot_ref(i_slot*);
    void call_on_enter_for_all_slots();
    void call_on_exit_for_all_slots();
};

////////////////////////////////////////////////////////////////////////////////

}

#endif // SM_I_SLOT_REF_LIST_HPP
