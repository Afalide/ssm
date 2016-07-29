
#pragma once

#include <queue>
//#include <thread>
#include <condition_variable>
#include <mutex>

struct i_event
{
    i_event(){}
    virtual ~i_event(){}
    virtual void call() = 0;
};

template <typename _context, typename _event_value>
struct basic_event
    : public i_event
{
    _context* m_ctx;
    _event_value m_ev;

    basic_event(_context* ctx, _event_value ev) : m_ctx(ctx), m_ev(ev) {}
    virtual ~basic_event(){}

    virtual void call() override
    {
        m_ctx->current_state_handle(m_ev);
    }
};

struct event_processor
{
    event_processor(){}
    virtual ~event_processor(){}

    virtual bool has_next() = 0;
    virtual void handle_next() = 0;
    virtual void handle_all() = 0;
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
        if (m_queue.size() == 0)
            return;

        i_event* ev = m_queue.front();
        ev->call();
        delete ev;
        m_queue.pop();
    }

    void handle_all() override
    {
        while(has_next())
            handle_next();
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
    void post(_context* context, _event_value evvalue)
    {
        {   // lock scope
            std::lock_guard<std::mutex> lock(m_guard_mx);
            i_event* ev = new basic_event<_context,_event_value>(context, evvalue);
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
        std::lock_guard<std::mutex> lock(m_guard_mx);

        if (m_queue.size() == 0)
            return;

        i_event* ev = m_queue.front();
        ev->call();
        delete ev;
        m_queue.pop();
    }

    void handle_all() override
    {
        while(has_next())
            handle_next();
    }

    void wait_handle_next()
    {
        while(! has_next())
            m_cv.wait(m_unique_lk);

        handle_next();
    }
};
