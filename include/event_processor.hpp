
#pragma once

#include <iostream>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <event_result.hpp>

struct i_event
{
    i_event(){}
    virtual ~i_event(){}
    virtual int call() = 0;
};

template <typename _context, typename _event_value>
struct basic_event
    : public i_event
{
    _context* m_ctx;
    _event_value m_ev;

    basic_event(_context* ctx, _event_value ev) : m_ctx(ctx), m_ev(ev) {}
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

struct event_processor
{
    bool m_active;

    event_processor() : m_active(true) {}
    virtual ~event_processor(){}

    virtual bool has_next() = 0;
    virtual void handle_next() = 0;

    virtual void set_active(bool act)
    {
        m_active = act;
    }

    virtual bool is_active()
    {
        return m_active;
    }

};

struct st_event_processor 
    : public event_processor
{
    std::queue<i_event*> m_queue;

    st_event_processor() : m_queue() {}
    virtual ~st_event_processor() {}

    template <typename _context, typename _event_value>
    void post(_context* context, _event_value evvalue)
    {
        i_event* ev = new basic_event<_context,_event_value>(context, evvalue);
        m_queue.push(ev);
    }

    bool has_next() override
    {
        return m_queue.size() > 0;
    }

    void handle_next() override
    {
        std::cout << "handle_next" << std::endl;
        if (m_queue.size() == 0)
            return;

        i_event* ev = m_queue.front();
        m_queue.pop();

        ev->call();
        delete ev;
    }
};

struct mt_event_processor 
    : public event_processor
{
    std::queue<i_event*>            m_queue;
    std::mutex                      m_unique_mx;
    std::mutex                      m_guard_mx;
    std::unique_lock<std::mutex>    m_unique_lk;
    std::condition_variable         m_cv;

    mt_event_processor() 
        : m_queue()
        , m_unique_mx()
        , m_guard_mx()
        , m_unique_lk(m_unique_mx)
        , m_cv() 
    {
    }

    virtual ~mt_event_processor() {}

    template <typename _context, typename _event_value>
    void post_basic_event(_context* context, _event_value evvalue)
    {
        {   // lock scope
            std::lock_guard<std::mutex> lock(m_guard_mx);
            i_event* ev = new basic_event<_context,_event_value>(context, evvalue);
            m_queue.push(ev);
        }

        m_cv.notify_one();
    }

    template <typename _context, typename _new_state>
    void post_transit_event(_context* context)
    {
        {   // lock scope
            std::lock_guard<std::mutex> lock(m_guard_mx);
            i_event* ev = new transit_event<_context,_new_state>(context);
            m_queue.push(ev);
        }

        m_cv.notify_one();
    }

    bool has_next() override
    {
        std::lock_guard<std::mutex> lock(m_guard_mx);
        return m_queue.size() > 0;
    }

    void handle_next() override
    {
        if (! is_active())
            return;

        i_event* ev = nullptr;

        {   // lock scope
            std::lock_guard<std::mutex> lock(m_guard_mx);

            if (m_queue.size() == 0)
                return;

            ev = m_queue.front();
            m_queue.pop();
        }

        /*int result =*/ ev->call();
        delete ev;

        //switch(result)
        //{
        //    case TINYSM_RESULT_EVENT_DONE:
        //        std::cout << "TINYSM_RESULT_EVENT_DONE" << std::endl;
        //        break;

        //    case TINYSM_RESULT_TRANSIT_DONE:
        //        std::cout << "TINYSM_RESULT_TRANSIT_DONE" << std::endl;
        //        break;

        //    case TINYSM_RESULT_REQ_PROCESSOR_STOP:
        //    {
        //        { // lock scope
        //            std::lock_guard<std::mutex> lock(m_guard_mx);
        //            m_stopped = true;
        //        }
        //        std::cout << "TINYSM_RESULT_REQ_PROCESSOR_STOP" << std::endl;
        //        break;
        //    }

        //    default:
        //        std::cout << "unknown event result" << std::endl;
        //}
    }

    void wait_handle_next()
    {
        while(! has_next())
            m_cv.wait(m_unique_lk);

        handle_next();
    }

    virtual void set_active(bool act) override
    {
        std::lock_guard<std::mutex> lock(m_guard_mx);
        event_processor::set_active(act);
    }

    virtual bool is_active() override
    {
        std::lock_guard<std::mutex> lock(m_guard_mx);
        return event_processor::is_active();
    }
};
