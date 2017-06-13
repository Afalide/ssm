
#ifndef AUTO_SINGLETON_HPP
#define AUTO_SINGLETON_HPP

#include <cassert>

template <typename t_>
struct auto_singleton
{
    static t_* m_instance;

    auto_singleton()
    {
//        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    virtual ~auto_singleton()
    {
    }

    static t_* auto_instance()
    {
//        static t_* ret = new t_;
//        assert(nullptr == m_instance);
//        m_instance = new t_;
//        return ret;

        if(nullptr == m_instance)
            m_instance = new t_;

        return m_instance;
    }

    static void clear_instance()
    {
        if(nullptr == m_instance)
            return;

        delete m_instance;
        m_instance = nullptr;
    }
};

template <typename t_>
t_* auto_singleton<t_>::m_instance = nullptr;

#endif // AUTO_SINGLETON_HPP

