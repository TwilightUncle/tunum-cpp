#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_DEDUCTION_GUIDE_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_DEDUCTION_GUIDE_HPP

#include TUNUM_COMMON_INCLUDE(floating/fe_fn.hpp)

namespace tunum
{
    // gccで親クラスのコンストラクタの型推論してくれなかったので、推論補助
    template <std::floating_point T>
    floating_std_info(T v)
        -> floating_std_info<T, std::numeric_limits<T>>;

    template <std::floating_point T>
    floating_std_info(const fe_holder<T>& v)
        -> floating_std_info<T, std::numeric_limits<T>>;

    // コールバック関数コンストラクタ用の推論補助
    template <class Lambda, class... Args> 
    requires (std::invocable<Lambda, Args...>)
    fe_holder(Lambda, Args...)
        -> fe_holder<std::invoke_result_t<Lambda, Args...>>;
}


#endif
