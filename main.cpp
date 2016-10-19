
#include <iostream>
//#include "State.hpp"
#include "context.hpp"
#include <Windows.h>
#include "protected_resource.hpp"

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
    : public protected_resource<Data*>::access_predicate
{
public:
	virtual bool operator()() override
	{
        std::cout << "testing predicate" << std::endl;
        return m_res->available();
	}
};

Data*                      g_data;
protected_resource<Data*>*  g_protected_data;

void thread_main()
{


    ////pdata->lock();
    ////pdata->get()->increase_value();
    ////pdata->release();

    //while(true)
    //{
    //    //pdata->get()->increase_value();
    //    std::cout << get_thread_id_str() << "waiting" << std::endl;
    //    pdata->wait_lock();
    //    pdata->get()->increase_value();
    //    //std::cout << get_thread_id_str() << "wait lock obtained" << std::endl;
    //    pdata->release();
    //}
    
    DataPredicate pred = g_protected_data->create_accessible_predicate<DataPredicate>();

	while(true)
	{
        //g_protected_data->wait_lock(pred);
        auto guard = g_protected_data->wait_lock_guard(pred);
		g_protected_data->get()->process_event();
		//g_protected_data->release();
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
    g_protected_data = new protected_resource<Data*>(g_data);

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
            g_protected_data->notify();
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

    g_protected_data->lock();
    delete g_protected_data->get();
    g_protected_data->release();
    delete g_protected_data;
    //delete g_pred

    return 0;
}
