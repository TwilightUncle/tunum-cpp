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
    };
}

#endif
