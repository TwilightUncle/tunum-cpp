#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MATH_ADD_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MATH_ADD_HPP

#include TUNUM_COMMON_INCLUDE(math/fe_fn.hpp)

namespace tunum
{
    // 加算
    template <std::floating_point Arg1, std::floating_point Arg2>
    struct add : public fe_fn<Arg1, Arg2>
    {
        using parent_t = fe_fn<Arg1, Arg2>;
        using calc_t = typename parent_t::calc_t;

        constexpr add(const fe_holder<Arg1>& arg1, const fe_holder<Arg2>& arg2)
            : parent_t(
                [](calc_t l, calc_t r) { return l + r; },
                arg1,
                arg2
            )
        {}
        constexpr add(Arg1 arg1, Arg2 arg2)
            : add(fe_holder{arg1}, fe_holder{arg2})
        {}
        template <std::integral _Arg1, std::integral _Arg2>
        constexpr add(_Arg1 arg1, _Arg2 arg2)
            : add(static_cast<double>(arg1), static_cast<double>(arg1))
        {}
    };

    // 整数の場合の推論
    template <std::integral Arg1, std::integral Arg2>
    add(Arg1, Arg2)
        -> add<double, double>;
}

#endif
