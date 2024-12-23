#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_SUB_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_SUB_HPP

#include TUNUM_COMMON_INCLUDE(floating/add.hpp)

namespace tunum
{
    template <std::floating_point Arg1, std::floating_point Arg2, std::fexcept_t RaiseFeFlags>
    struct sub : public add<Arg1, Arg2, RaiseFeFlags>
    {
        using parent_t = add<Arg1, Arg2, RaiseFeFlags>;
        using calc_t = typename parent_t::calc_t;
        using info_t = typename parent_t::info_t;

        constexpr sub(const fe_holder<Arg1, RaiseFeFlags>& arg1, const fe_holder<Arg2, RaiseFeFlags>& arg2)
            : parent_t(arg1, -arg2)
        {}
    };
}

#endif
