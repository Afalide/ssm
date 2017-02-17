
#include "state_a1.hpp"

#include <iostream>

void 
state_a1::on_enter()
{
    std::cout << "state A1  ...enter" << std::endl;
}

void 
state_a1::on_exit()
{
    std::cout << "state A1  ...exit" << std::endl;
}
