
#include <iostream>
//#include "State.hpp"
#include "Context.hpp"


/*
#include "ProtectedResource.hpp"

struct Data
{
    int val;
    Data() : val(0) {}
    void increase_value()
    {
        val += 5;
        std::cout << GetThreadIdStr() << "increased value to " << val << std::endl;
    }
};

 ProtectedResource<Data*> *pdata;

void thread_main()
{
    //pdata->Lock();
    //pdata->Get()->increase_value();
    //pdata->Release();

    while(true)
    {
        //pdata->Get()->increase_value();
        std::cout << GetThreadIdStr() << "waiting" << std::endl;
        pdata->Wait();
        pdata->Get()->increase_value();
        //std::cout << GetThreadIdStr() << "wait lock obtained" << std::endl;
        pdata->Release();
    }
}

#define THREADS_COUNT 10
*/

struct Eva{};
struct Evb{};
struct Evc{};

struct MyEventInterface
{
    virtual ~MyEventInterface(){}
    void handle(Eva){std::cout << "MyEventInterface default handle Eva" << std::endl;}
    void handle(Evb){std::cout << "MyEventInterface default handle Evb" << std::endl;}
    void handle(Evc){std::cout << "MyEventInterface default handle Evc" << std::endl;}
};

struct StateIdle : public MyEventInterface
                 , public state<MyEventInterface>
{
    StateIdle()  {std::cout << "StateIdle enter" << std::endl;}
    ~StateIdle() {std::cout << "StateIdle exit" << std::endl;}

    void handle(Eva){std::cout << "StateIdle handle Eva" << std::endl;}
};

struct MyContext : public context<MyEventInterface>
{
    
};

int main()
{
    MyContext ctx;
    ctx.perform_transit<StateIdle>();



    /*
    pdata = new ProtectedResource<Data*>(new Data);

    std::thread threads[THREADS_COUNT];

    for(int i=0; i<THREADS_COUNT; ++i)
        threads[i] = std::thread(thread_main);

    char c;
    bool loop = true;

    while(loop)
    {
        std::cin >> c;

        switch(c)
        {
        case 'n':
            pdata->Notify();
            break;

        case 'q':
            loop = false;
            break;

        default:
            continue;
        }
    }

    for(int i=0; i<THREADS_COUNT; ++i)
        threads[i].join();

    pdata->Lock();
    delete pdata->Get();
    pdata->Release();
    delete pdata;
    */
    
    

    return 0;
}
