
#include "tests.hpp"
#include "protected_resource.hpp"

#include <cassert>
#include <thread>

struct test1_data
{
    bool accessed;

    test1_data()
        : accessed (false)
    { }

    void manipulate()
    {
        if(accessed)
        {
            // another thread is already accessing this resource
            assert(false);
        }
        accessed = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        accessed = false;
    }
};

typedef protected_resource<test1_data*> pres_test1_data;

void thread_main(pres_test1_data* pres)
{
    for(unsigned int i=0; i<TINYSM_TEST_THREAD_PULSES; ++i)
    {
        pres->lock();
        pres->get()->manipulate();
        pres->release();
    }
}

void test1()
{
    // Race condition test: make sure the data structure is being accessed only once
    // at a time

    pres_test1_data res(new test1_data);
    std::thread threads[TINYSM_TEST_THREAD_COUNT];

    for(unsigned int i=0; i<TINYSM_TEST_THREAD_COUNT; ++i)
        threads[i] = std::thread(thread_main, &res);

    for(unsigned int i=0; i<TINYSM_TEST_THREAD_COUNT; ++i)
        threads[i].join();
}


