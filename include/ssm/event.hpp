
#ifndef SSM_EVENT_HPP
#define SSM_EVENT_HPP

#include <type_traits>

////////////////////////////////////////////////////////////////////////////////

template <typename T>
void
register_event ()
{
    std::cout << "registering event " << T::name () << '\n';
}

////////////////////////////////////////////////////////////////////////////////

template < typename T, 
           typename... REM,
           std::enable_if_t <sizeof... (REM) != 0, bool> = true>
void
register_event ()
{
    // std::cout << "registering event " << T::name () << '\n';
    register_event <T> ();
    register_event <REM...> ();
}

////////////////////////////////////////////////////////////////////////////////

template <typename... ARGS>
struct event
{
    void register_all ()
    {
        register_event <ARGS...> ();
    }
};

////////////////////////////////////////////////////////////////////////////////

#endif
