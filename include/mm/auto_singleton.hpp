
#ifndef MANIACMACHINES_AUTO_SINGLETON_HPP
#define MANIACMACHINES_AUTO_SINGLETON_HPP

////////////////////////////////////////////////////////////////////////////////

#include <cassert>

////////////////////////////////////////////////////////////////////////////////

namespace mm {

////////////////////////////////////////////////////////////////////////////////

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
        if(nullptr == m_instance)
            m_instance = new t_;

        return m_instance;
    }

    static void clear()
    {
        if(nullptr == m_instance)
            return;

        delete m_instance;
        m_instance = nullptr;
    }
};

template <typename t_>
t_* auto_singleton<t_>::m_instance = nullptr;

////////////////////////////////////////////////////////////////////////////////

} // namespace mm

////////////////////////////////////////////////////////////////////////////////

#endif //MANIACMACHINES_AUTO_SINGLETON_HPP
