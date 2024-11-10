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

    // コールバック関数コンストラクタ用の推論補助
    template <class Lambda, class... Args> 
    requires (std::invocable<Lambda, Args...>)
    fe_holder(Lambda, Args...)
        -> fe_holder<std::invoke_result_t<Lambda, Args...>>;

    // 加算における整数の場合の推論
    template <std::integral Arg1, std::integral Arg2>
    add(Arg1, Arg2)
        -> add<double, double>;
    template <std::floating_point Arg1, std::integral Arg2>
    add(Arg1, Arg2)
        -> add<Arg1, double>;
    template <std::integral Arg1, std::floating_point Arg2>
    add(Arg1, Arg2)
        -> add<double, Arg2>;
}


#endif
