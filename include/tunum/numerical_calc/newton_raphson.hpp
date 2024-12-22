#ifndef TUNUM_INCLUDE_GUARD_TUNUM_NUMERICAL_CALC_NEWTON_RAPHSON_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_NUMERICAL_CALC_NEWTON_RAPHSON_HPP

#include <stdexcept>
#include TUNUM_COMMON_INCLUDE(math/abs.hpp)

namespace tunum
{
    // ------------------------------------
    // ニュートン法
    // ------------------------------------

    template <class F, class DF>
    struct newton_raphson
    {
        // 近似対象の関数
        F func;
        // func を微分した関数
        DF d_func;

        constexpr newton_raphson(F&& f, DF&& df) noexcept
            : func(f)
            , d_func(df)
        {}

        constexpr newton_raphson(const F& f, const DF& df) noexcept
            : func(f)
            , d_func(df)
        {}

        // 関数の近似値を算出する
        // @param init 初期値
        // @param converged_diff 収束したとする差
        template <class T>
        requires (TuFloatingInvocable<F, T, T> && TuFloatingInvocable<DF, T, T>)
        constexpr T resolve(const T& init, const T& converged_diff = T{}) const
        {
            if (converged_diff < 0)
                throw std::invalid_argument("Argment converged_diff cannot have a value less than zero.");

            T x = init;
            while (true) {
                // ニュートン法の次の値を算出
                const T before_x = x;
                x -= func(x) / d_func(x);

                // 収束判定
                const T diff = abs(x - before_x);
                // const T diff_abs = (std::max)(diff, -diff);
                if (diff <= converged_diff)
                    break;
            }
            return x;
        }
    };
}

#endif
