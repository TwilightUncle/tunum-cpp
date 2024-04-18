#include <gtest/gtest.h>
#include <tunum/floating.hpp>

TEST(TunumFloatingTest, StdInfoTest)
{
    using float_limit = std::numeric_limits<float>;
    using double_limit = std::numeric_limits<double>;
    constexpr auto info1 = tunum::floating_std_info{0.};
    constexpr auto info2 = tunum::floating_std_info{float(-1)};
    constexpr auto info3 = tunum::floating_std_info{float_limit::quiet_NaN()};
    constexpr auto info4 = tunum::floating_std_info{double_limit::signaling_NaN()};
    constexpr auto info5 = tunum::floating_std_info{float_limit::infinity()};
    constexpr auto info6 = tunum::floating_std_info{-double_limit::infinity()};
    constexpr auto info7 = tunum::floating_std_info{double_limit::denorm_min()};
    constexpr auto info8 = tunum::floating_std_info{-float_limit::denorm_min()};
    constexpr auto info9 = tunum::floating_std_info{.1};
    constexpr auto info10 = tunum::floating_std_info{-1.1};
    constexpr auto info11 = tunum::floating_std_info{float(11)};
    EXPECT_TRUE(info1.is_zero());
    EXPECT_TRUE(info1.sign() > 0);
    EXPECT_FALSE(info1.has_decimal_part());
    EXPECT_TRUE(info2.is_normalized());
    EXPECT_TRUE(info2.sign() < 0);
    EXPECT_EQ(info2.mantissa_bits(), 0);
    EXPECT_EQ(info2.exponent(), 0);
    EXPECT_FALSE(info2.has_decimal_part());
    EXPECT_TRUE(info3.is_nan());
    EXPECT_FALSE(info3.has_decimal_part());
    EXPECT_TRUE(info4.is_nan());
    EXPECT_FALSE(info4.has_decimal_part());
    EXPECT_TRUE(info5.sign() > 0);
    EXPECT_TRUE(info5.is_infinity());
    EXPECT_FALSE(info5.has_decimal_part());
    EXPECT_TRUE(info6.sign() < 0);
    EXPECT_TRUE(info6.is_infinity());
    EXPECT_FALSE(info6.has_decimal_part());
    EXPECT_TRUE(info7.sign() > 0);
    EXPECT_TRUE(info7.is_denormalized());
    EXPECT_TRUE(info7.has_decimal_part());
    EXPECT_TRUE(info8.sign() < 0);
    EXPECT_TRUE(info8.is_denormalized());
    EXPECT_TRUE(info8.has_decimal_part());
    EXPECT_TRUE(info9.sign() > 0);
    EXPECT_TRUE(info9.is_normalized());
    EXPECT_TRUE(info9.has_decimal_part());
    EXPECT_TRUE(info10.sign() < 0);
    EXPECT_TRUE(info10.is_normalized());
    EXPECT_TRUE(info10.has_decimal_part());
    EXPECT_TRUE(info11.sign() > 0);
    EXPECT_TRUE(info11.is_normalized());
    EXPECT_FALSE(info11.has_decimal_part());
}
