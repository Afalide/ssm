
#include "tests.hpp"

#include <cassert>
#include "protected_resource.hpp"

void do_all_tests()
{
    test1();
}

//struct data
//{
//
//};

//#define protected_resource_manual_locks_thread_count 10
//
//typedef protected_resource<data*> t_pr_data;
//t_pr_data* g_protected;
//
//void protected_resource_manual_locks_thread_main()
//{
//    while(true)
//    {
//
//    }
//}
//
//void protected_resource_manual_locks()
//{
//    g_protected = new t_pr_data(new data);
//    
//
//    
//}
