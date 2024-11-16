#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_DEDUCTION_GUIDE_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_DEDUCTION_GUIDE_HPP

#include TUNUM_COMMON_INCLUDE(floating/sub.hpp)
#include TUNUM_COMMON_INCLUDE(floating/mul.hpp)
#include TUNUM_COMMON_INCLUDE(floating/div.hpp)

namespace tunum
{
    // --------------------------------------------------------------
    // 組み込みの浮動小数点型解釈オブジェクトの推論補助
    // gccで親クラスのコンストラクタの型推論してくれなかったので、推論補助
    // --------------------------------------------------------------

    template <std::floating_point T>
    floating_std_info(T v)
        -> floating_std_info<T>;

    // fe_holderからの推論
    template <std::floating_point T>
    floating_std_info(const fe_holder<T>& v)
        -> floating_std_info<T>;

    // --------------------------------------------------------------
    // 浮動小数点例外を保持するオブジェクトの推論補助
    // --------------------------------------------------------------

    // 整数型用
    template <std::integral T>
    fe_holder(T)
        -> fe_holder<double, std::fexcept_t{}>;
    template <std::integral T>
    fe_holder(T, std::fexcept_t)
        -> fe_holder<double, std::fexcept_t{}>;

    // コールバック関数コンストラクタ用
    template <class Lambda, class... Args> 
    requires (std::invocable<Lambda, Args...>)
    fe_holder(Lambda, Args...)
        -> fe_holder<std::invoke_result_t<Lambda, Args...>, std::fexcept_t{}>;

    // --------------------------------------------------------------
    // 加算の推論
    // --------------------------------------------------------------

    template <class Arg1, std::floating_point Arg2, std::fexcept_t RaiseFeFlags>
    requires (std::is_arithmetic_v<Arg1>)
    add(Arg1, fe_holder<Arg2, RaiseFeFlags>)
        -> add<integral_to_floating_t<Arg1, Arg2>, Arg2, RaiseFeFlags>;
    template <std::floating_point Arg1, class Arg2, std::fexcept_t RaiseFeFlags>
    requires (std::is_arithmetic_v<Arg2>)
    add(fe_holder<Arg1, RaiseFeFlags>, Arg2)
        -> add<Arg1, integral_to_floating_t<Arg2, Arg1>, RaiseFeFlags>;
    template <class Arg1, class Arg2>
    requires (std::is_arithmetic_v<Arg1> && std::is_arithmetic_v<Arg2>)
    add(Arg1, Arg2)
        -> add<integral_to_floating_t<Arg1, Arg2>, integral_to_floating_t<Arg2, Arg1>, std::fexcept_t{}>;

    // --------------------------------------------------------------
    // 減算の推論補助
    // --------------------------------------------------------------

    template <class Arg1, std::floating_point Arg2, std::fexcept_t RaiseFeFlags>
    requires (std::is_arithmetic_v<Arg1>)
    sub(Arg1, fe_holder<Arg2, RaiseFeFlags>)
        -> sub<integral_to_floating_t<Arg1, Arg2>, Arg2, RaiseFeFlags>;
    template <std::floating_point Arg1, class Arg2, std::fexcept_t RaiseFeFlags>
    requires (std::is_arithmetic_v<Arg2>)
    sub(fe_holder<Arg1, RaiseFeFlags>, Arg2)
        -> sub<Arg1, integral_to_floating_t<Arg2, Arg1>, RaiseFeFlags>;
    template <class Arg1, class Arg2>
    requires (std::is_arithmetic_v<Arg1> && std::is_arithmetic_v<Arg2>)
    sub(Arg1, Arg2)
        -> sub<integral_to_floating_t<Arg1, Arg2>, integral_to_floating_t<Arg2, Arg1>, std::fexcept_t{}>;

    // --------------------------------------------------------------
    // 乗算の推論補助
    // --------------------------------------------------------------

    template <class Arg1, std::floating_point Arg2, std::fexcept_t RaiseFeFlags>
    requires (std::is_arithmetic_v<Arg1>)
    mul(Arg1, fe_holder<Arg2, RaiseFeFlags>)
        -> mul<integral_to_floating_t<Arg1, Arg2>, Arg2, RaiseFeFlags>;
    template <std::floating_point Arg1, class Arg2, std::fexcept_t RaiseFeFlags>
    requires (std::is_arithmetic_v<Arg2>)
    mul(fe_holder<Arg1, RaiseFeFlags>, Arg2)
        -> mul<Arg1, integral_to_floating_t<Arg2, Arg1>, RaiseFeFlags>;
    template <class Arg1, class Arg2>
    requires (std::is_arithmetic_v<Arg1> && std::is_arithmetic_v<Arg2>)
    mul(Arg1, Arg2)
        -> mul<integral_to_floating_t<Arg1, Arg2>, integral_to_floating_t<Arg2, Arg1>, std::fexcept_t{}>;

    // --------------------------------------------------------------
    // 除算の推論補助
    // --------------------------------------------------------------

    template <class Arg1, std::floating_point Arg2, std::fexcept_t RaiseFeFlags>
    requires (std::is_arithmetic_v<Arg1>)
    div(Arg1, fe_holder<Arg2, RaiseFeFlags>)
        -> div<integral_to_floating_t<Arg1, Arg2>, Arg2, RaiseFeFlags>;
    template <std::floating_point Arg1, class Arg2, std::fexcept_t RaiseFeFlags>
    requires (std::is_arithmetic_v<Arg2>)
    div(fe_holder<Arg1, RaiseFeFlags>, Arg2)
        -> div<Arg1, integral_to_floating_t<Arg2, Arg1>, RaiseFeFlags>;
    template <class Arg1, class Arg2>
    requires (std::is_arithmetic_v<Arg1> && std::is_arithmetic_v<Arg2>)
    div(Arg1, Arg2)
        -> div<integral_to_floating_t<Arg1, Arg2>, integral_to_floating_t<Arg2, Arg1>, std::fexcept_t{}>;
}

#endif
