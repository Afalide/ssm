
// This file is part of the Tales Mesa City Engine
// See the copyright notice tmce.license for more informations

#ifndef TMCE_CORE_SINGLETON_HPP
#define TMCE_CORE_SINGLETON_HPP

#include <cassert>

#define TMCE_SINGLETON(class_name) template <> class_name* tmce::singleton<class_name>::_instance = nullptr;

namespace tmce
{

template <typename T>
class singleton
{

public:

    singleton();
    virtual ~singleton();

    static T* get_instance();

protected:

    static T* _instance;
};

template <typename T>
singleton<T>::singleton()
{
    assert(nullptr == _instance);
    _instance = static_cast<T*>(this);
}

template <typename T>
singleton<T>::~singleton()
{
    assert(nullptr != _instance);
    _instance = nullptr;
}

template <typename T>
T*
singleton<T>::get_instance()
{
    return _instance;
}

}

#endif // TMCE_CORE_SINGLETON_HPP