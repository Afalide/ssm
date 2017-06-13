
#include "state_a1.hpp"
#include "state_a2.hpp"

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

void
state_a1::handle(ev_a1_foo)
{
    std::cout << "state A1  ev_a1_foo" << std::endl;
    transit<state_a2>();
}
