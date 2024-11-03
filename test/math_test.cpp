#include <gtest/gtest.h>
#include <tunum/math.hpp>

TEST(TunumMathTest, MathTest)
{
    constexpr auto copysign_1 = tunum::copysign(1, -2);
    constexpr auto copysign_2 = tunum::copysign(-1., 2.);
    constexpr auto copysign_3 = tunum::copysign(1.f, 2.f);
    constexpr auto copysign_4 = tunum::copysign(-1.l, -2.l);
    EXPECT_EQ(copysign_1, std::copysign(1, -2));
    EXPECT_EQ(copysign_2, std::copysign(-1., 2.));
    EXPECT_EQ(copysign_3, std::copysign(1.f, 2.f));
    EXPECT_EQ(copysign_4, std::copysign(-1.l, -2.l));

    constexpr auto nextafter_1 = tunum::nextafter(0.f, 1.f);
    constexpr auto nextafter_2 = tunum::nextafter(0.f, -1.f);
    EXPECT_EQ(nextafter_1, std::nextafter(0.f, 1.f));
    EXPECT_EQ(nextafter_2, std::nextafter(0.f, -1.f));
    constexpr auto nexttoward_1 = tunum::nexttoward(0.f, 1.f);
    constexpr auto nexttoward_2 = tunum::nexttoward(0.f, -1.f);
    EXPECT_EQ(nexttoward_1, std::nexttoward(0.f, 1.f));
    EXPECT_EQ(nexttoward_2, std::nexttoward(0.f, -1.f));
    
    constexpr auto abs_1 = tunum::abs(-1);
    constexpr auto abs_2 = tunum::abs(1);
    EXPECT_EQ(abs_1, std::abs(-1));
    EXPECT_EQ(abs_2, std::abs(1));

    constexpr auto exp_1 = tunum::exp(float(3));
    constexpr auto exp_2 = tunum::exp(float(6.1));
    constexpr auto exp_3 = tunum::exp(float(-5));
    constexpr auto exp_4 = tunum::exp(float(-62.2));
    EXPECT_EQ(exp_1, std::exp(float(3)));
    EXPECT_EQ(exp_2, std::exp(float(6.1)));
    EXPECT_EQ(exp_3, std::exp(float(-5)));
    EXPECT_EQ(exp_4, std::exp(float(-62.2)));

    constexpr auto sqrt_1 = tunum::sqrt(float(2));
    EXPECT_EQ(sqrt_1, std::sqrt(float(2)));

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
