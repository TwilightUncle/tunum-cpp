#ifndef TUNUM_INCLUDE_GUARD_TUNUM_NUMERICAL_CALC_NEWTON_RAPHSON_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_NUMERICAL_CALC_NEWTON_RAPHSON_HPP

#include <stdexcept>
#include TUNUM_COMMON_INCLUDE(concepts.hpp)

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
        // @param sigma 収束したとする誤差
        template <TuArithmetic T>
        requires (TuArithmeticInvocable<F, T, T> && TuArithmeticInvocable<DF, T, T>)
        constexpr T resolve(const T& init, const T& sigma = T{}) const
        {
            if (sigma < 0)
                throw std::invalid_argument("Argment sigma cannot have a value less than zero.");

            T x = init;
            while (true) {
                // ニュートン法の次の値を算出
                const T before_x = x;
                x -= func(x) / d_func(x);

                // 誤差の範囲を判定
                const T diff = x - before_x;
                const T diff_abs = (std::max)(diff, -diff);
                if (diff_abs <= sigma)
                    break;
            }
            return x;
        }
    };
}

#endif
