
#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <string>
#include <type_traits>

std::string GetThreadIdStr();

// TODO: hide ("encapsulate") the ProtectedResource logic
// (mostly mutexes and locks) inside a non-template base class.
// This is preferable to avoid the user pulling in all the
// mutex and thread-related headers.

// TODO: this is getting larger than expected, add unit testing

// TODO: refactor this file with snake case

class Predicate
{
public:
	virtual ~Predicate() { }
	virtual bool operator()() = 0; // The predicate implementation goes here.
	                               // This function returns the predicate value: true or false.
	                               // This function is executed with a locked mutex: be 
	                               // careful not to call any Lock-like function inside
	                               // this one.
};

class DefaultPredicate : public Predicate
{
public:

    virtual bool operator()() override
    {
        return true; // The default predicate (used as default parameters for some calls)
                     // which is always true.
    }
};

template <typename TResource>
class ProtectedResource
{
    typedef std::lock_guard  <std::mutex> glock;
    typedef std::unique_lock <std::mutex> ulock;

    TResource                mRes;
    std::mutex               mMutex;
    std::condition_variable  mCv;
    ulock*                   mCurrentLock;

#if defined(TINYSM_STRONG_CHECKS)
    std::thread::id mLockerId;    
#endif

    ProtectedResource(const ProtectedResource& other);
    ProtectedResource& operator= (const ProtectedResource&);

public:

    ProtectedResource(TResource res) 
        : mRes(res)
        , mMutex()
        , mCv()
        , mCurrentLock(nullptr)
#if defined(TINYSM_STRONG_CHECKS)
        , mLockerId()
#endif
    {
    }

    virtual ~ProtectedResource()
    {
        if(nullptr != mCurrentLock)
        {
            std::cout << GetThreadIdStr() << "error, deleting a ProtectedResource while the resource is still being locked" << std::endl;
            
            // Try to unlock anyways; the following might fail / crash / throw

            mCurrentLock->release();
            delete mCurrentLock;
            mMutex.unlock();
        }
    }

    TResource Get()
    {
#if defined(TINYSM_STRONG_CHECKS)
        if(std::this_thread::get_id() != mLockerId)
        {
            std::cout << GetThreadIdStr() << "error, the thread attempting to access the resource is not the one which locked it" << std::endl;
            // Maybe throw something
        }
#endif

        if(nullptr == mCurrentLock)
        {
            std::cout << GetThreadIdStr() << "error, accessing released resource" << std::endl;
            // Maybe throw something
        }

        std::cout << GetThreadIdStr() << "obtaining resource" << std::endl;
        return mRes;
    }

    void Release()
    {

        #if defined(TINYSM_STRONG_CHECKS)
        if(std::this_thread::get_id() != mLockerId)
        {
            std::cout << GetThreadIdStr() << "error, the thread attempting to release the resource is not the one which locked it" << std::endl;
            // Maybe throw something
            return;
        }
        #endif

        if(nullptr == mCurrentLock)
        {
            std::cout << GetThreadIdStr() << "warning, resource already released" << std::endl;
            return;
        }

        // Make sure the member is set to null BEFORE releasing the lock

        ulock* lock_to_delete = mCurrentLock;
        mCurrentLock = nullptr;

        #if defined(TINYSM_STRONG_CHECKS)
        mLockerId = std::thread::id();
        #endif

        delete lock_to_delete; // Unlocks
    }

    void LockImpl(bool wait, bool release_on_wake, Predicate& wait_predicate)
    {
        #if defined(TINYSM_STRONG_CHECKS)
        if(mLockerId == std::this_thread::get_id())
        {
            std::cout << GetThreadIdStr() << "error, the thread is attempting to lock the same resource twice" << std::endl;
            // Maybe throw something
        }
        #endif

        ulock* lock = new ulock(mMutex); // May block

		if(wait)
		{
			while(! wait_predicate())
                mCv.wait(*lock); 
		}

		if(wait && release_on_wake)
		{
			delete lock; // Release the lock after wake up, the resource is not locked
			             // and therefore not owned by the user.
			return;
		}

        mCurrentLock = lock;

        #if defined(TINYSM_STRONG_CHECKS)
        mLockerId = std::this_thread::get_id();
        #endif
    }

    void Lock()
    {
		// Get a lock, do not sleep
        DefaultPredicate wait_predicate;
        LockImpl(false, false, wait_predicate);
    }

    void WaitLock(Predicate& wait_predicate)
    {
		// Sleep until wake-up, and get a lock
        LockImpl(true, false, wait_predicate);
    }

	void Wait(Predicate& wait_predicate)
	{
		// Sleep until wake-up, no lock obtained
		LockImpl(true, true, wait_predicate);
	}

    void Notify()
    {
        glock lock(mMutex);
        std::cout << "notifying" << std::endl;
        mCv.notify_all();
    }

    // A predicate which already has an access to the protected 
    // resource value. This is because most of custom predicates need
    // to access the resource in order to work (ex: a predicate which 
    // determines if a vector is available would need to look
    // at the vector's size)
    
    class AccessPredicate
        : public Predicate
    {
        friend class ProtectedResource<TResource>;
    protected:
        TResource mResource;
    public:
        AccessPredicate() : mResource(nullptr) { }
        AccessPredicate(const AccessPredicate& other) : mResource(other.mResource) { }
        AccessPredicate& operator= (const AccessPredicate& other) { mResource = other.mResource; }
        virtual ~AccessPredicate() { }
        virtual bool operator()() = 0;
        
    };

    template <typename TAccessPredicate>
    TAccessPredicate CreateAccessiblePredicate()
    {
        static_assert ( std::is_base_of<AccessPredicate, TAccessPredicate>::value
                      , "Your predicate must derive from ProtectedResource::AccessPredicate");
        TAccessPredicate ap;
        ap.mResource = mRes;
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
        Lock();
        return LockGuard(mCurrentLock);
    }

    LockGuard WaitLockGuard(Predicate& predicate)
    {
        WaitLock(predicate);
        return LockGuard(mCurrentLock);
    }
};
