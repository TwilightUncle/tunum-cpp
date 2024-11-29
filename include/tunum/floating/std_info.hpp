#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_STD_INFO_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_STD_INFO_HPP

#include <stdexcept>
#include TUNUM_COMMON_INCLUDE(floating/bit_info.hpp)

namespace tunum
{
    template <std::floating_point T>
    struct floating_std_info
        : public floating_bit_info<T, std::numeric_limits<T>>
    {
        using limits_t = std::numeric_limits<T>;
        using parent_t = floating_bit_info<T, limits_t>;
        using data_store_t = typename parent_t::data_store_t;
        using exponent_value_t = typename parent_t::exponent_value_t;

        // 
        static_assert(limits_t::radix == 2);
        static_assert(limits_t::is_iec559);

        // ----------------------------------------------
        // コンストラクタ
        // ----------------------------------------------

        // 親クラスで定義されているコンストラクタは全て使用可能とする
        using floating_bit_info<T, limits_t>::floating_bit_info;

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
            if (floating_std_info{y}.is_nan())
                return y;
            return y < T(*this)
                ? (T)this->next(-1.f)
                : (T)this->next(static_cast<float>(y != T(*this)));
        }

        // 無限大の値を取得
        // @param signbit 符号ビットの指定
        static constexpr T get_infinity(bool signbit = false) noexcept
        { return (T)parent_t::make_infinity(signbit); }

        // 非数の値を取得
        // @param signbit 符号ビットの指定
        static constexpr T get_nan(bool signbit = false) noexcept
        { return (T)parent_t::make_nan(signbit); }

        // ゼロの値を取得
        // @param signbit 符号ビットの指定
        static constexpr T get_zero(bool signbit = false) noexcept
        { return (T)parent_t::make_zero(signbit); }

        // 非正規化数の最小値を取得
        // @param signbit 符号ビットの指定
        static constexpr T get_denormalized_min(bool signbit = false) noexcept
        { return (T)parent_t::make_denormalized_min(signbit); }

        // 非正規化数の最大値を取得
        // @param signbit 符号ビットの指定
        static constexpr T get_denormalized_max(bool signbit = false) noexcept
        { return (T)parent_t::make_denormalized_max(signbit); }

        // 正規化数の最小値を取得
        // @param signbit 符号ビットの指定
        static constexpr T get_min(bool signbit = false) noexcept
        { return (T)parent_t::make_min(signbit); }

        // 正規化数の最大値を取得
        // @param signbit 符号ビットの指定
        static constexpr T get_max(bool signbit = false) noexcept
        { return (T)parent_t::make_max(signbit); }

        constexpr T ldexp(int exp) const noexcept
        { return (T)this->add_exponent(exp); }

        // 2の累乗関数(引数は整数のみバージョン)
        static constexpr T exp2_integral(int x) noexcept
        { return (T)floating_std_info{T{1}}.add_exponent(x); }
    };
}

#endif
