
#ifndef SSM_ERROR_HPP
#define SSM_ERROR_HPP

////////////////////////////////////////////////////////////////////////////////

#include <stdexcept>
#include <iostream>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////

#define SSM_ERROR(msg)                   \
{                                        \
    std::stringstream ss;                \
    ss << msg;                           \
    std::cerr << ss .str () << '\n';     \
    throw std::logic_error {ss .str ()}; \
}

////////////////////////////////////////////////////////////////////////////////

#endif
