
#ifndef SM_SLOT_HPP
#define SM_SLOT_HPP

#include <iostream>
#include "i_slot.hpp"


namespace sm {

////////////////////////////////////////////////////////////////////////////////

template <typename t_start_state>
struct slot
    : public i_slot
{
    slot()
    {
        switch_holder<t_start_state>(false);
//        switch_holder_no_entries<t_start_state>();
    }

    virtual ~slot()
    {
    }
};

////////////////////////////////////////////////////////////////////////////////

}

#endif // SM_SLOT_HPP
