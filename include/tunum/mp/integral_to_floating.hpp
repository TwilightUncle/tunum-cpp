#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MP_INTEGRAL_TO_FLOATING_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MP_INTEGRAL_TO_FLOATING_HPP

#include TUNUM_COMMON_INCLUDE(submodule_loader.hpp)

namespace tunum::tpfn
{
    // Fromが整数型の場合、整数型を浮動小数点型へ置き換える
    // Fromが整数型以外の場合、Fromの型をそのまま返却
    // From整数かつToが浮動小数点以外の場合はfloat型に変換
    // @tparam From 
    // @tparam To 右オペランド型
    template <class From, class To = float>
    struct integral_to_floating
        : public std::type_identity<From> {};
    template <std::integral From, class To>
    struct integral_to_floating<From, To>
        : public std::type_identity<float> {};
    template <std::integral From, std::floating_point To>
    struct integral_to_floating<From, To>
        : public std::type_identity<To> {};
}

namespace tunum
{
    // Fromが整数型の場合、整数型を浮動小数点型へ置き換える
    // Fromが整数型以外の場合、Fromの型をそのまま返却
    // From整数かつToが浮動小数点以外の場合はfloat型に変換
    // @tparam From 
    // @tparam To 右オペランド型
    using integral_to_floating = tump::cbk<tpfn::integral_to_floating, 2>;

    // Fromが整数型の場合、整数型をfloat型へ置き換える
    // Fromが整数型以外の場合、Fromの型をそのまま返却
    // @tparam From 
    using integral_to_float = tump::eval<tump::flip, integral_to_floating, float>;

    // Fromが整数型の場合、整数型を浮動小数点型へ置き換える
    // Fromが整数型以外の場合、Fromの型をそのまま返却
    // From整数かつToが浮動小数点以外の場合はfloat型に変換
    // @tparam From 
    // @tparam To 右オペランド型
    template <class From, class To = float>
    using integral_to_floating_t = typename tpfn::integral_to_floating<From, To>::type;
}

#endif
