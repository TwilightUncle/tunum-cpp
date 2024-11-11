#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_DEDUCTION_GUIDE_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_DEDUCTION_GUIDE_HPP

#include TUNUM_COMMON_INCLUDE(floating/add.hpp)

namespace tunum
{
    // gccで親クラスのコンストラクタの型推論してくれなかったので、推論補助
    template <std::floating_point T>
    floating_std_info(T v)
        -> floating_std_info<T, std::numeric_limits<T>>;

    // fe_holderからの推論
    template <std::floating_point T>
    floating_std_info(const fe_holder<T>& v)
        -> floating_std_info<T, std::numeric_limits<T>>;

    // 整数型用の推論補助
    template <std::integral T>
    fe_holder(T)
        -> fe_holder<double>;
    template <std::integral T>
    fe_holder(T, std::fexcept_t)
        -> fe_holder<double>;

    // コールバック関数コンストラクタ用の推論補助
    template <class Lambda, class... Args> 
    requires (std::invocable<Lambda, Args...>)
    fe_holder(Lambda, Args...)
        -> fe_holder<std::invoke_result_t<Lambda, Args...>>;

    // 加算の推論
    template <class Arg1, std::floating_point Arg2>
    requires (std::is_arithmetic_v<Arg1>)
    add(Arg1, fe_holder<Arg2>)
        -> add<integral_to_floating_t<Arg1, Arg2>, Arg2>;
    template <std::floating_point Arg1, class Arg2>
    requires (std::is_arithmetic_v<Arg2>)
    add(fe_holder<Arg1>, Arg2)
        -> add<Arg1, integral_to_floating_t<Arg2, Arg1>>;
    template <class Arg1, class Arg2>
    requires (std::is_arithmetic_v<Arg1> && std::is_arithmetic_v<Arg2>)
    add(Arg1, Arg2)
        -> add<integral_to_floating_t<Arg1, Arg2>, integral_to_floating_t<Arg2, Arg1>>;
}


#endif
