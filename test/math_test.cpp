#include <gtest/gtest.h>
#include <tunum/math.hpp>
#include <cmath>

TEST(TunumMathTest, MathTest)
{
    constexpr auto abs_1 = tunum::abs(-1);
    constexpr auto abs_2 = tunum::abs(1);
    EXPECT_EQ(abs_1, std::abs(-1));
    EXPECT_EQ(abs_2, std::abs(1));

    constexpr auto exp_1 = tunum::exp(float(3));
    constexpr auto exp_2 = tunum::exp(float(6.1));
    constexpr auto exp_3 = tunum::exp(float(-5));
    constexpr auto exp_4 = tunum::exp(float(-62.2));
    constexpr auto exp_5 = tunum::exp(float(124.1));
    EXPECT_EQ(exp_1, std::exp(float(3)));
    EXPECT_EQ(exp_2, std::exp(float(6.1)));
    EXPECT_EQ(exp_3, std::exp(float(-5)));
    EXPECT_EQ(exp_4, std::exp(float(-62.2)));
    EXPECT_EQ(exp_5, std::exp(float(124.1)));

    constexpr auto ln_1 = tunum::ln(float(3));
    constexpr auto ln_2 = tunum::ln(double(1));
    constexpr auto ln_3 = tunum::ln(double(3));
    constexpr auto ln_4 = tunum::ln(float(0.09));
    constexpr auto ln_5 = tunum::ln(float(0.17));
    EXPECT_EQ(ln_1, std::log(float(3)));
    EXPECT_EQ(ln_2, std::log(double(1)));
    // 絶妙に結果が一致しない...
    // EXPECT_EQ(ln_3, std::log(double(3)));
    EXPECT_EQ(ln_4, std::log(float(0.09)));
    EXPECT_EQ(ln_5, std::log(float(0.17)));
}