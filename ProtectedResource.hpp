
#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <string>

std::string GetThreadIdStr();

// TODO: hide ("encapsulate") the ProtectedResource logic
// (mostly mutexes and locks) inside a non-template base class.
// This is preferable to avoid the user pulling in all the
// mutex and thread-related headers.

// TODO: add a lock-guard-style accessor for the resource, to avoid
// repeated Lock + Release (or Wait + Release) invocations (this
// might lead to errors due to missed Release calls)
// ie:
//     MyClass c;
//     ProtectedResource<MyClass> prot(c);
//     prot.GetWithLockGuard().DoStuff();
//     // no need to call Release here

//TODO: do same thing with a Wait condition

template <typename TResource>
class ProtectedResource
{
    typedef std::lock_guard  <std::mutex> glock;
    typedef std::unique_lock <std::mutex> ulock;

    TResource mRes;
    std::mutex mMutex;
    std::condition_variable mCv;
    ulock* mCurrentLock;

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

    void LockImpl(bool wait)
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
            mCv.wait(*lock); // No loop, we are sensible to spurious wakes, but that's OK.
                             // It is up to the user to put the Wait() function inside a
                             // loop and check with it's own predicate.

        mCurrentLock = lock;

        #if defined(TINYSM_STRONG_CHECKS)
        mLockerId = std::this_thread::get_id();
        #endif
    }

    void Lock()
    {
        LockImpl(false);
    }

    void Wait()
    {
        LockImpl(true);
    }

    void Notify()
    {
        glock lock(mMutex);
        std::cout << "notifying" << std::endl;
        mCv.notify_all();
    }
};
