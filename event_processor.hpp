
#pragma once

#include <queue>
#include "ProtectedResource.hpp"
#include "singleton.hpp"

struct i_event
{
    i_event(){}
    virtual ~i_event(){}
    virtual int call() = 0;
};

template <typename _context, typename _event>
struct basic_event
    : public i_event
{
    _context* m_ctx;
    _event m_ev;

    basic_event(_context* ctx, _event ev) : m_ctx(ctx), m_ev(ev) {}
    virtual ~basic_event(){}

    virtual int call() override
    {
        return m_ctx->perform_current_state_handle(m_ev);
    }
};

template <typename _context, typename _new_state>
struct transit_event
    : public i_event
{
    _context* m_ctx;

    transit_event(_context* ctx) : m_ctx(ctx){}
    virtual ~transit_event(){}

    virtual int call() override
    {
        return m_ctx->perform_transit<_new_state>();
    }
};

class event_processor
    : public tmce::singleton<event_processor>
{
    typedef std::queue<i_event*> t_event_queue;
    typedef ProtectedResource<t_event_queue*> t_protected_event_vector;

    //t_event_queue* m_queue;
    t_protected_event_vector m_prot_queue;

    class event_queue_predicate
        : public t_protected_event_vector::AccessPredicate
    {
        virtual bool operator()()
        {
            return mResource->size() > 0;
        }
    };

public:

    event_processor()
        //: m_queue(new t_event_queue)
        : m_prot_queue(new t_event_queue)
    {

    }

    ~event_processor()
	{
        
	}

    void append(i_event* ev)
    {
        m_prot_queue.Lock();
        m_prot_queue.Get()->push(ev);
        m_prot_queue.Release();
    }

    template <typename _context, typename _event>
    void post_basic_event(_context* ctx, _event ev)
    {
        i_event* ev = new basic_event<_context,_event>(ctx, ev);
        append(ev);
    }

    template <typename _context, typename _new_state>
    void post_transit_event(_context* context)
    {
        i_event* ev = new transit_event<_context,_new_state>(context);
        append(ev);
    }

    void wait_process_next()
    {
        Predicate& pred = m_prot_queue.CreateAccessiblePredicate<event_queue_predicate>();
        
        // Wait until an event is available and lock the queue

        m_prot_queue.WaitLock(pred);

        // Get an event

        t_event_queue* queue  = m_prot_queue.Get();
        i_event* ev           = queue->front();
        
        // Process the event

        queue->pop();
        ev->call();

        delete ev;

        // Unlock the queue

        m_prot_queue.Release();
    }

	//bool has_next()
	//{
	//	m_res.Lock();
	//	return st_has_next();
	//	m_res.Release();
	//}

	//void process_next()
	//{
	//	m_res.Lock();
	//	
	//	if(! st_has_next())
	//		return;

	//	t_event_queue* vec = m_res.Get();

	//	i_event* ev = vec->front();
	//	vec->pop();

	//	m_res.Release();
	//
	//	// queue unlocked, process the event

	//	ev->call();
	//}

	//void block_and_process_all()
	//{
	//	while(true)
	//	{
	//		if(! has_next())
	//		{
	//			m_res.Wait();
	//			continue;
	//		}

	//		m_res.Lock();

	//		m_res.Release();
	//	}
	//}

private:

	//bool st_has_next()
	//{
	//	return m_res.Get()->size() > 0;
	//}
};
