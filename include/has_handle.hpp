
#pragma once

template <typename _class, typename _ret, /*typename... _params*/ typename _param>
struct has_handle
{
    static_assert(!std::is_same<void, _ret>::value, "Template argument 'ret' is  of type 'void', which is unsupported");

    typedef char ok[1];
    typedef char no[2];

    template <typename T>
    static ok& check(decltype(std::declval<T>().handle(std::declval<_param>()/*std::declval<_params>()...*/)));

    template <typename T>
    static no& check(...);

    static const bool value = sizeof(check<_class>(std::declval<Ret>())) == sizeof(ok);
};
