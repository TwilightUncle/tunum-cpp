#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_STD_INFO_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_STD_INFO_HPP

#include TUNUM_COMMON_INCLUDE(floating/bit_info.hpp)

namespace tunum
{
    template <std::floating_point T, class LimitsT = std::numeric_limits<T>>
    requires (LimitsT::radix == 2 && LimitsT::is_iec559)
    struct floating_std_info
        : public floating_bit_info<(sizeof(T) << 3), LimitsT::digits - 1, LimitsT::max_exponent - 1>
    {
        // numeric_limitではケチ表現の分、1桁多いためマイナス1
        using parent_t = floating_bit_info<(sizeof(T) << 3), LimitsT::digits - 1, LimitsT::max_exponent - 1>;
        using limits_t = LimitsT;
        using data_store_t = typename parent_t::data_store_t;
        using exponent_value_t = typename parent_t::exponent_value_t;

        // ----------------------------------------------
        // コンストラクタ
        // ----------------------------------------------

        constexpr floating_std_info() : parent_t() {}
        constexpr floating_std_info(T v) noexcept
            : parent_t(std::bit_cast<data_store_t, T>(v))
        {}

        // ----------------------------------------------
        // メンバ関数群
        // ----------------------------------------------

        constexpr explicit operator T() const noexcept
        { return std::bit_cast<T, data_store_t>(this->data); }

        // 整数部分取得
        constexpr T get_integral_part() const noexcept
        { return std::bit_cast<T, data_store_t>(this->get_integral_part_bits()); }

        // 小数部分取得
        // bit解釈だとずれが出るため、組み込みの演算を用いて、整数部分を引く
        constexpr T get_decimal_part() const noexcept
        {
            // 無限大は符号部のみ残して、0を返却
            if (this->is_infinity())
                return std::bit_cast<T, data_store_t>(this->data & this->sign_mask);
            // 整数部が存在しないことが自明または、正規化数以外はそのまま値を返却
            return !this->is_normalized() || this->exponent() < 0
                ? T(*this)
                : T(*this) - get_integral_part();
        }
    };
}

#endif
