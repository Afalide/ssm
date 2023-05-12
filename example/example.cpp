
#include <ssm/state_machine.hpp>

struct foo : public state { };

int main ()
{
    state_machine sm;
    
    sm .add_root_state <foo> ("Foo");
    
    return 0;
}
