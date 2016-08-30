
#include "ProtectedResource.hpp"

#include <sstream>

std::string GetThreadIdStr()
{
    std::stringstream ss;
    ss << std::this_thread::get_id();
    char buf[12];
    sprintf_s(buf, "[%8.8s] ", ss.str().c_str());
    return std::string(buf);
}