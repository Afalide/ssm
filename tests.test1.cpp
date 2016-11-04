
#include "tests.hpp"
#include "protected_resource.hpp"

#include <cassert>
#include <thread>
#include <map>

struct data_helper
{
    bool accessed;

    data_helper()
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
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        accessed = false;
    }
};

struct wait_helper
{
    bool manipulated;
    bool pred_observer_ready;

    wait_helper()
        : manipulated (false)
        , pred_observer_ready (false)
    { }

    void manipulate()
    {
        manipulated = true;
    }
};

typedef protected_resource<data_helper*> pres_data_helper;
typedef protected_resource<wait_helper*> pres_wait_helper;

struct wait_helper_observer_ready_predicate
    : public pres_wait_helper::access_predicate
{
    bool operator()() override
    {
        return m_res->pred_observer_ready;
    }
};

pres_wait_helper* g_pres_wait_helper;

void thread_main_lock_and_release(pres_data_helper* pres)
{
    for(unsigned int i=0; i<TINYSM_TEST_THREAD_PULSES; ++i)
    {
        pres->lock();
        pres->get()->manipulate();
        pres->release();
    }
}

void thread_main_lock_guard(pres_data_helper* pres)
{
    for(unsigned int i=0; i<TINYSM_TEST_THREAD_PULSES; ++i)
    {
        auto guard = pres->get_lock_guard();
        pres->get()->manipulate();
    }
}

void thread_main_wait_performer()
{
    auto pred = g_pres_wait_helper->create_accessible_predicate<wait_helper_observer_ready_predicate>();
    g_pres_wait_helper->wait_lock(pred);
    g_pres_wait_helper->get()->manipulate();
    g_pres_wait_helper->release();
}

void thread_main_wait_observer()
{
    // Give time to the wait performer thread to do put itself on wait
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // We are ready to see and observe

    {
        auto lock = g_pres_wait_helper->get_lock_guard();
        g_pres_wait_helper->get()->pred_observer_ready = true;
    }

    // Make sure the data has not been tampered with

    {
        auto lock = g_pres_wait_helper->get_lock_guard();
        assert(false == g_pres_wait_helper->get()->manipulated);
    }

    // The thread performer should be waiting by now, wake it up
    
    g_pres_wait_helper->notify();

    // Give time to the wait performer thread to manipulate the data helper
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
        auto lock = g_pres_wait_helper->get_lock_guard();
        assert(true == g_pres_wait_helper->get()->manipulated);
    }
}

void thread_main_wait()
{
    // The manipulated data helper

    g_pres_wait_helper = new pres_wait_helper(new wait_helper);

    // Starts 2 threads: the one which does the wait, and the one which
    // observes the waiting behavior

    std::thread th_wait_performer (thread_main_wait_performer);
    std::thread th_wait_observer  (thread_main_wait_observer);
    
    th_wait_performer.join();
    th_wait_observer.join();

    g_pres_wait_helper->lock();
    delete g_pres_wait_helper->get();
    delete g_pres_wait_helper; // will call unlock()
}

struct multiple_waits_helper
{
    typedef std::map<std::thread::id, unsigned short>  t_counter_map;
    typedef std::pair<std::thread::id, unsigned short> t_counter_map_entry;
    
    t_counter_map m_count_map;

    multiple_waits_helper()
        : m_count_map()
    { }

    void access()
    {
        auto id = std::this_thread::get_id();

        if (0 == m_count_map.count(id))
        {
            m_count_map.insert(t_counter_map_entry(id, 1));
        }
        else
        {
            unsigned short& count = m_count_map.at(id); 
            assert(count < TINYSM_TEST_THREAD_PULSES); // each thread should access the resource no more than X times
            ++count;
        }
    }
};

typedef protected_resource<multiple_waits_helper*> pres_multiple_waits_helper;

void thread_main_do_some_waits(pres_multiple_waits_helper* res)
{
    for(unsigned int i = 0; i<TINYSM_TEST_THREAD_PULSES; ++i)
    {
        auto guard = res->get_lock_guard();
        res->get()->access();
    }
}

//void thread_main_instantiate_threads_whith_multiple_waits()
//{
//
//}

void test1()
{
    std::cout << "begin test 1" << std::endl;

    pres_data_helper res(new data_helper);
    std::thread threads[TINYSM_TEST_THREAD_COUNT];

    std::cout << "test 1 lock and release" << std::endl;

    for(unsigned int i=0; i<TINYSM_TEST_THREAD_COUNT; ++i)
        threads[i] = std::thread(thread_main_lock_and_release, &res);

    for(unsigned int i=0; i<TINYSM_TEST_THREAD_COUNT; ++i)
        threads[i].join();

    std::cout << "test 1 lock guard" << std::endl;

    for(unsigned int i=0; i<TINYSM_TEST_THREAD_COUNT; ++i)
        threads[i] = std::thread(thread_main_lock_guard, &res);

    for(unsigned int i=0; i<TINYSM_TEST_THREAD_COUNT; ++i)
        threads[i].join();

    std::cout << "test 1 wait" << std::endl;

    std::thread th_wait (thread_main_wait);
    th_wait.join();

    std::cout << "test 1 multiple waits" << std::endl;

    pres_multiple_waits_helper pres_multiple_waits (new multiple_waits_helper);

    {
        auto guard = pres_multiple_waits.get_lock_guard();
        assert(0 == pres_multiple_waits.get()->m_count_map.size());
    }

    for(unsigned int i=0; i<TINYSM_TEST_THREAD_COUNT; ++i)
        threads[i] = std::thread(thread_main_do_some_waits, &pres_multiple_waits);

    for(unsigned int i=0; i<TINYSM_TEST_THREAD_COUNT; ++i)
        threads[i].join();
    {
        auto guard = pres_multiple_waits.get_lock_guard();
        assert(TINYSM_TEST_THREAD_COUNT == pres_multiple_waits.get()->m_count_map.size());

        multiple_waits_helper::t_counter_map& map = pres_multiple_waits.get()->m_count_map;
        for (auto it=map.begin(); it!=map.end(); ++it)
        {
            auto entry = *it;
            assert(TINYSM_TEST_THREAD_PULSES == entry.second);
        }
    }

    std::cout << "test 1 cleanup" << std::endl;

    res.lock();
    delete res.get();
    res.release();

    pres_multiple_waits.lock();
    delete pres_multiple_waits.get();
    pres_multiple_waits.release();

    std::cout << "test 1 done" << std::endl;
}
