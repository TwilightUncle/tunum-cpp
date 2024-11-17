#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MP_IS_ARITHMETIC_OR_FE_HOLDER_ONLY_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MP_IS_ARITHMETIC_OR_FE_HOLDER_ONLY_HPP

#include TUNUM_COMMON_INCLUDE(mp/is_fe_holder.hpp)

namespace tunum::tpfn
{
    // リストの中身がすべて算術型または、fe_holderであること
    // @tparam T 型のテンプレートパラメータを持つ具体型
    template <class T>
    struct is_arithmetic_or_fe_holder_only;

    template <template <class...> class ListT, class... Types>
    struct is_arithmetic_or_fe_holder_only<ListT<Types...>>
        : public std::bool_constant<(... && (std::is_arithmetic_v<Types> || is_fe_holder_v<Types>))>
    {};
}

namespace tunum
{
    // リストの中身がすべて算術型または、fe_holderであること
    // @tparam T 型のテンプレートパラメータを持つ具体型
    using is_arithmetic_or_fe_holder_only = tump::cbk<tpfn::is_arithmetic_or_fe_holder_only, 1>;

    // リストの中身がすべて算術型または、fe_holderであること
    // @tparam T 型のテンプレートパラメータを持つ具体型
    template <class T>
    constexpr bool is_arithmetic_or_fe_holder_only_v = tpfn::is_arithmetic_or_fe_holder_only<T>::value;
}

#endif
