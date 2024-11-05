#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_STD_INFO_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_STD_INFO_HPP

#include <stdexcept>
#include TUNUM_COMMON_INCLUDE(floating/bit_info.hpp)

namespace tunum
{
    template <std::floating_point T, class LimitsT = std::numeric_limits<T>>
    requires (LimitsT::radix == 2 && LimitsT::is_iec559)
    struct floating_std_info
        : public floating_bit_info<T, LimitsT>
    {
        using parent_t = floating_bit_info<T, LimitsT>;
        using limits_t = LimitsT;
        using data_store_t = typename parent_t::data_store_t;
        using exponent_value_t = typename parent_t::exponent_value_t;

        // ----------------------------------------------
        // コンストラクタ
        // ----------------------------------------------

        // 親クラスで定義されているコンストラクタは全て使用可能とする
        using floating_bit_info<T, LimitsT>::floating_bit_info;

        // ----------------------------------------------
        // メンバ関数群
        // ----------------------------------------------

        // 整数部分取得
        constexpr T get_integral_part() const noexcept
        { return (T)this->make_integral_part(); }

        // 小数部分取得
        // bit解釈だとずれが出るため、組み込みの演算を用いて、整数部分を引く
        constexpr T get_decimal_part() const noexcept
        { return (T)this->make_decimal_part(); }

        // 隣接するbit表現の値を取得
        constexpr T nextafter(T y) const
        {
            return floating_std_info{y}.is_nan()
                ? y
                : (T)this->next(y - T(*this));
        }

        constexpr T ldexp(int exp) const noexcept
        { return (T)this->add_exponent(exp); }

        // 2の累乗関数(引数は整数のみバージョン)
        static constexpr T exp2_integral(int x) noexcept
        { return (T)floating_std_info{T{1}}.add_exponent(x); }
    };

    // gccで親クラスのコンストラクタの型推論してくれなかったので、推論補助
    template <std::floating_point T>
    floating_std_info(T v)
        -> floating_std_info<T, std::numeric_limits<T>>;
}

#endif
