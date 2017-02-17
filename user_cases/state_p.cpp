
#include "state_p.hpp"

#include <iostream>

void
state_p::on_enter()
{
    std::cout << "state P  .enter" << std::endl;
}

void
state_p::on_exit()
{
    std::cout << "state P  .exit" << std::endl;
}
