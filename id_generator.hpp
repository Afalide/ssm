
#ifndef ID_GENERATOR_HPP
#define ID_GENERATOR_HPP

struct unique_int
{
    static int value;
};

template <typename _t>
struct identifier
{
    static int get_id()
    {
        static int ret = unique_int::value ++;
        return ret;
    }
};

#endif // ID_GENERATOR_HPP
