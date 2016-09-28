
#pragma once

#include <queue>
#include "protected_resource.hpp"
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
    typedef protected_resource<t_event_queue*> t_protected_event_vector;

    //t_event_queue* m_queue;
    t_protected_event_vector m_prot_queue;

    class event_queue_predicate
        : public t_protected_event_vector::access_predicate
    {
        virtual bool operator()()
        {
            return m_res->size() > 0;
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
        m_prot_queue.lock();
        m_prot_queue.get()->push(ev);
        m_prot_queue.release();
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
        event_queue_predicate pred = m_prot_queue.create_accessible_predicate<event_queue_predicate>();
        
        // wait until an event is available and lock the queue

        m_prot_queue.wait_lock(pred);

        // get an event

        t_event_queue* queue  = m_prot_queue.get();
        i_event* ev           = queue->front();
        
        // Process the event

        queue->pop();
        ev->call();

        delete ev;

        // Unlock the queue

        m_prot_queue.release();
    }

	//bool has_next()
	//{
	//	m_res.lock();
	//	return st_has_next();
	//	m_res.release();
	//}

	//void process_next()
	//{
	//	m_res.lock();
	//	
	//	if(! st_has_next())
	//		return;

	//	t_event_queue* vec = m_res.get();

	//	i_event* ev = vec->front();
	//	vec->pop();

	//	m_res.release();
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
	//			m_res.wait();
	//			continue;
	//		}

	//		m_res.lock();

	//		m_res.release();
	//	}
	//}

private:

	//bool st_has_next()
	//{
	//	return m_res.get()->size() > 0;
	//}
};
