
#include <iostream>
//#include "State.hpp"
#include "context.hpp"
#include <Windows.h>
#include "ProtectedResource.hpp"

struct Data
{
private:

    int val;

public:

    Data() 
        : val(0)
    {
	}

    ~Data()
    {
    }

	std::queue<int> list;

	bool available()
	{
		return list.size() > 0;
	}

	void add_event()
	{
		int val = rand();
		list.push(val);
		std::cout << "pushed event val " << val << std::endl;
	}

    void process_event()
    {
		val = list.front();
		Sleep(100);
		std::cout << "processed event val " << val << std::endl;
		list.pop();
    }
};

class DataPredicate
    : public ProtectedResource<Data*>::AccessPredicate
{
public:
	virtual bool operator()() override
	{
        std::cout << "testing predicate" << std::endl;
        return mResource->available();
	}
};

Data*                      g_data;
ProtectedResource<Data*>*  g_protected_data;

void thread_main()
{


    ////pdata->Lock();
    ////pdata->Get()->increase_value();
    ////pdata->Release();

    //while(true)
    //{
    //    //pdata->Get()->increase_value();
    //    std::cout << GetThreadIdStr() << "waiting" << std::endl;
    //    pdata->WaitLock();
    //    pdata->Get()->increase_value();
    //    //std::cout << GetThreadIdStr() << "wait lock obtained" << std::endl;
    //    pdata->Release();
    //}
    
    DataPredicate pred = g_protected_data->CreateAccessiblePredicate<DataPredicate>();

	while(true)
	{
        g_protected_data->WaitLock(pred);
		g_protected_data->Get()->process_event();
		g_protected_data->Release();
	}
}

#define THREADS_COUNT 3


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
    //MyContext ctx;
    //ctx.perform_transit<StateIdle>();

    g_data = new Data;
    //g_pred = new DataPredicate(g_data);
    g_protected_data = new ProtectedResource<Data*>(g_data);

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
            g_protected_data->Notify();
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

    g_protected_data->Lock();
    delete g_protected_data->Get();
    g_protected_data->Release();
    delete g_protected_data;
    //delete g_pred

    return 0;
}
