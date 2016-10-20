
#include "tests/tests.hpp"
#include "protected_resource.hpp"

struct test1_data
{
    bool accessed;

    test1_data()
        : accessed (false)
    { }
};

// Race condition test: make sure the data structure is being accessed only once
// at a time

void test1()
{
    protected_resource <test1_data*> res(new test1_data);

    
}
