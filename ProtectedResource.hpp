
#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <string>
#include <type_traits>

std::string get_thread_id_str();

// TODO: hide ("encapsulate") the protected_resource logic
// (mostly mutexes and locks) inside a non-template base class.
// This is preferable to avoid the user pulling in all the
// mutex and thread-related headers.

// TODO: this is getting larger than expected, add unit testing

// TODO: refactor this file with snake case

class predicate
{
public:
	virtual ~predicate() { }
	virtual bool operator()() = 0; // The predicate implementation goes here.
	                               // This function returns the predicate value: true or false.
	                               // This function is executed with a locked mutex: be 
	                               // careful not to call any lock-like function inside
	                               // this one.
};

class default_predicate : public predicate
{
public:

    virtual bool operator()() override
    {
        return true; // The default predicate (used as default parameters for some calls)
                     // which is always true.
    }
};

template <typename t_resource>
class protected_resource
{
    typedef std::lock_guard  <std::mutex> glock;
    typedef std::unique_lock <std::mutex> ulock;

    t_resource                m_res;
    std::mutex               m_mutex;
    std::condition_variable  m_cv;
    ulock*                   m_current_lock;

#if defined(TINYSM_STRONG_CHECKS)
    std::thread::id m_locker_id;    
#endif

    protected_resource(const protected_resource& other);
    protected_resource& operator= (const protected_resource&);

public:

    protected_resource(t_resource res) 
        : m_res(res)
        , m_mutex()
        , m_cv()
        , m_current_lock(nullptr)
#if defined(TINYSM_STRONG_CHECKS)
        , m_locker_id()
#endif
    {
    }

    virtual ~protected_resource()
    {
        if(nullptr != m_current_lock)
        {
            std::cout << get_thread_id_str() << "error, deleting a protected_resource while the resource is still being locked" << std::endl;
            
            // Try to unlock anyways; the following might fail / crash / throw

            m_current_lock->release();
            delete m_current_lock;
            m_mutex.unlock();
        }
    }

    t_resource get()
    {
        #if defined(TINYSM_STRONG_CHECKS)
        if(std::this_thread::get_id() != m_locker_id)
        {
            std::cout << get_thread_id_str() << "error, the thread attempting to access the resource is not the one which locked it" << std::endl;
            // Maybe throw something
        }
        #endif

        if(nullptr == m_current_lock)
        {
            std::cout << get_thread_id_str() << "error, accessing released resource" << std::endl;
            // Maybe throw something
        }

        std::cout << get_thread_id_str() << "obtaining resource" << std::endl;
        return m_res;
    }

    void release()
    {
        #if defined(TINYSM_STRONG_CHECKS)
        if(std::this_thread::get_id() != m_locker_id)
        {
            std::cout << get_thread_id_str() << "error, the thread attempting to release the resource is not the one which locked it" << std::endl;
            // Maybe throw something
            return;
        }
        #endif

        if(nullptr == m_current_lock)
        {
            std::cout << get_thread_id_str() << "warning, resource already released" << std::endl;
            return;
        }

        // Make sure the member is set to null BEFORE releasing the lock

        ulock* lock_to_delete = m_current_lock;
        m_current_lock = nullptr;

        #if defined(TINYSM_STRONG_CHECKS)
        m_locker_id = std::thread::id();
        #endif

        delete lock_to_delete; // Unlocks
    }

    void lock_impl(bool wait, bool release_on_wake, predicate& wait_predicate)
    {
        #if defined(TINYSM_STRONG_CHECKS)
        if(m_locker_id == std::this_thread::get_id())
        {
            std::cout << get_thread_id_str() << "error, the thread is attempting to lock the same resource twice" << std::endl;
            // Maybe throw something
        }
        #endif

        ulock* lock = new ulock(m_mutex); // May block

		if(wait)
		{
			while(! wait_predicate())
                m_cv.wait(*lock); 
		}

		if(wait && release_on_wake)
		{
			delete lock; // Release the lock after wake up, the resource is not locked
			             // and therefore not owned by the user.
			return;
		}

        m_current_lock = lock;

        #if defined(TINYSM_STRONG_CHECKS)
        m_locker_id = std::this_thread::get_id();
        #endif
    }

    void lock()
    {
		// get a lock, do not sleep
        default_predicate wait_predicate; // Default predicate, always true. Unused anyway since
                                          // there is no wait.
        lock_impl(false, false, wait_predicate);
    }

    void wait_lock(predicate& wait_predicate)
    {
		// Sleep until wake-up, and get a lock
        lock_impl(true, false, wait_predicate);
    }

	void wait(predicate& wait_predicate)
	{
		// Sleep until wake-up, no lock obtained
		lock_impl(true, true, wait_predicate);
	}

    void notify()
    {
        glock lock(m_mutex);
        std::cout << "notifying" << std::endl;
        m_cv.notify_all();
    }

    // A predicate which already has an access to the protected 
    // resource value. This is because most of custom predicates need
    // at least to read the resource in order to work (ex: a predicate which 
    // determines if a vector is available would need to look
    // at the vector's size)
    // Remember that a predicate is always executed in a locked context,
    // therefore any resource modification is assumed to be thread-safe.
    
    class access_predicate
        : public predicate
    {
        friend class protected_resource<t_resource>;
    protected:
        t_resource m_res;
    public:
        access_predicate() : m_res(nullptr) { }
        access_predicate(const access_predicate& other) : m_res(other.m_res) { }
        access_predicate& operator= (const access_predicate& other) { m_res = other.m_res; }
        virtual ~access_predicate() { }
        virtual bool operator()() = 0;
    };

    template <typename t_access_predicate>
    t_access_predicate create_accessible_predicate()
    {
        static_assert ( std::is_base_of<access_predicate, t_access_predicate>::value
                      , "Your predicate must derive from protected_resource::access_predicate");
        t_access_predicate ap;
        ap.m_res = m_res;
        return ap;
    }

    // A structure which mimics the lock guard behavior.
    // The lock is released upon destruction as a destructor side-effect.
    // Because there are no mechanisms such as automatic reference counting,
    // any copy created from this structure invalidates the old one.
    // This is to make sure there is only one structure responsible
    // of the lock guard deletion.

    struct LockGuard
    {
        ulock* m_lock;

        LockGuard(ulock* lock)
            : m_lock(lock)
        { }

        LockGuard(LockGuard& other)
            : m_lock(nullptr)
        {
            BuildFrom(other);
            other.Clear();
        }

        LockGuard(LockGuard&& other)
            : m_lock(nullptr)
        {
            BuildFrom(other);
            other.Clear();
        }

        ~LockGuard()
        {
            if(nullptr != m_lock)
                delete m_lock;

            Clear();
        }

        LockGuard& operator=(const LockGuard& other)
        {
            BuildFrom(other);
            other.Clear();
        }

        LockGuard& operator=(LockGuard&& other)
        {
            BuildFrom(other);
            other.Clear();
        }

        void BuildFrom(const LockGuard& other)
        {
            m_lock = other.m_lock;
        }

        void Clear()
        {
            m_lock = nullptr;
        }
    };

    LockGuard GetLockGuard()
    {
        lock();
        return LockGuard(m_current_lock);
    }

    LockGuard WaitLockGuard(predicate& predicate)
    {
        wait_lock(predicate);
        return LockGuard(m_current_lock);
    }
};
