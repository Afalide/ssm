
#include "state_a2.hpp"

#include <iostream>

void 
state_a2::on_enter()
{
    std::cout << "state A2  ...enter" << std::endl;
}

void 
state_a2::on_exit()
{
    std::cout << "state A2  ...exit" << std::endl;
}
