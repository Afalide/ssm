
#include <iostream>
//#include "State.hpp"
#include "context.hpp"
#include <Windows.h>
#include "ProtectedResource.hpp"
#include <ctime>
#include <cstdlib>

struct Data
{
    int val;
    Data() : val(0) 
	{
		srand(time(NULL));
	}

	std::queue<int> list;

	bool available()
	{
		return list.size > 0;
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
	: public Predicate
{
public:

	Data* mData;

	DataPredicate(Data* dat)
		: mData(dat)
	{ }

	virtual bool operator()() override
	{
		return mData->available();
	}
};

 ProtectedResource<Data*>* pdata;

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

	while(true)
	{
		pdata->WaitLock();
		pdata->Get()->process_event();
		pdata->Release();
	}
}

#define THREADS_COUNT 2


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




	Data* data = new Data;
	DataPredicate* pred = new DataPredicate(data);
    
    pdata = new ProtectedResource<Data*>(data, pred);

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

    return 0;
}
