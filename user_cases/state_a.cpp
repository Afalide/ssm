
#include "state_a.hpp"

#include <iostream>

void 
state_a::on_enter()
{
    std::cout << "state A  ..enter" << std::endl;
}

void 
state_a::on_exit()
{
    std::cout << "state A  ...exit" << std::endl;
}
