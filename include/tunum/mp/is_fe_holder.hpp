#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MP_IS_FE_HOLDER_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MP_IS_FE_HOLDER_HPP

#include <cfenv>
#include TUNUM_COMMON_INCLUDE(submodule_loader.hpp)

namespace tunum
{
    // 前方宣言
    template <std::floating_point T, std::fexcept_t RaiseFeFlags>
    struct fe_holder;
}

namespace tunum::tpfn
{
    // fe_holderかどうか判定
    // @tparam T 検査対象型
    template <class T>
    struct is_fe_holder : public std::false_type {};

    // fe_holder
    template <std::floating_point T, std::fexcept_t RaiseFeFlags>
    struct is_fe_holder<fe_holder<T, RaiseFeFlags>> : public std::true_type {};

    // fe_holderを継承した型1
    template <template <auto, class...> class T, std::floating_point... ArgsT, std::fexcept_t RaiseFeFlags>
    struct is_fe_holder<T<RaiseFeFlags, ArgsT...>> : public std::is_base_of<
        fe_holder<tump::mp_max_t<tump::list<ArgsT...>>, RaiseFeFlags>,
        T<RaiseFeFlags, ArgsT...>
    > {};

    // fe_holderを継承した型
    // ※一部コンパイラでエラーとなるため、1 ~ 5つの引数の型をハードコーディング。
    // (さすがに、5以上の引数を要する数学関数はないでしょ)
    template <template <class, auto> class T, std::floating_point ArgT, std::fexcept_t RaiseFeFlags>
    struct is_fe_holder<T<ArgT, RaiseFeFlags>> : public std::is_base_of<
        fe_holder<ArgT, RaiseFeFlags>,
        T<ArgT, RaiseFeFlags>
    > {};
    template <
        template <class, class, auto> class T,
        std::floating_point ArgT1,
        std::floating_point ArgT2,
        std::fexcept_t RaiseFeFlags
    >
    struct is_fe_holder<T<ArgT1, ArgT2, RaiseFeFlags>> : public std::is_base_of<
        fe_holder<tump::mp_max_t<tump::list<ArgT1, ArgT2>>, RaiseFeFlags>,
        T<ArgT1, ArgT2, RaiseFeFlags>
    > {};
    template <
        template <class, class, class, auto> class T,
        std::floating_point ArgT1,
        std::floating_point ArgT2,
        std::floating_point ArgT3,
        std::fexcept_t RaiseFeFlags
    >
    struct is_fe_holder<T<ArgT1, ArgT2, ArgT3, RaiseFeFlags>> : public std::is_base_of<
        fe_holder<tump::mp_max_t<tump::list<ArgT1, ArgT2, ArgT3>>, RaiseFeFlags>,
        T<ArgT1, ArgT2, ArgT3, RaiseFeFlags>
    > {};
    template <
        template <class, class, class, class, auto> class T,
        std::floating_point ArgT1,
        std::floating_point ArgT2,
        std::floating_point ArgT3,
        std::floating_point ArgT4,
        std::fexcept_t RaiseFeFlags
    >
    struct is_fe_holder<T<ArgT1, ArgT2, ArgT3, ArgT4, RaiseFeFlags>> : public std::is_base_of<
        fe_holder<tump::mp_max_t<tump::list<ArgT1, ArgT2, ArgT3, ArgT4>>, RaiseFeFlags>,
        T<ArgT1, ArgT2, ArgT3, ArgT4, RaiseFeFlags>
    > {};
    template <
        template <class, class, class, class, class, auto> class T,
        std::floating_point ArgT1,
        std::floating_point ArgT2,
        std::floating_point ArgT3,
        std::floating_point ArgT4,
        std::floating_point ArgT5,
        std::fexcept_t RaiseFeFlags
    >
    struct is_fe_holder<T<ArgT1, ArgT2, ArgT3, ArgT4, ArgT5, RaiseFeFlags>> : public std::is_base_of<
        fe_holder<tump::mp_max_t<tump::list<ArgT1, ArgT2, ArgT3, ArgT4, ArgT5>>, RaiseFeFlags>,
        T<ArgT1, ArgT2, ArgT3, ArgT4, ArgT5, RaiseFeFlags>
    > {};
}

namespace tunum
{
    // fe_holderかどうか判定
    // @tparam T 検査対象型
    using is_fe_holder = tump::cbk<tpfn::is_fe_holder, 1>;

    // fe_holderかどうか判定
    // @tparam T 検査対象型
    template <class T>
    constexpr bool is_fe_holder_v = tpfn::is_fe_holder<T>::value;
}

#endif
