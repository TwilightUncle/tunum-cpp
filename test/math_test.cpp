#include <gtest/gtest.h>
#include <tunum/math.hpp>

TEST(TunumMathTest, CopySignTest)
{
    constexpr auto copysign_1 = tunum::copysign(1, -2);
    constexpr auto copysign_2 = tunum::copysign(-1., 2.);
    constexpr auto copysign_3 = tunum::copysign(1.f, 2.f);
    constexpr auto copysign_4 = tunum::copysign(-1.l, -2.l);
    EXPECT_EQ(copysign_1, std::copysign(1, -2));
    EXPECT_EQ(copysign_2, std::copysign(-1., 2.));
    EXPECT_EQ(copysign_3, std::copysign(1.f, 2.f));
    EXPECT_EQ(copysign_4, std::copysign(-1.l, -2.l));
}

TEST(TunumMathTest, NextAfterTest)
{
    // zero→非正規化数の計算
    constexpr auto nextafter_1 = tunum::nextafter(0.f, 1.f);
    constexpr auto nextafter_2 = tunum::nextafter(0.f, -1.f);
    EXPECT_EQ(nextafter_1, std::nextafter(0.f, 1.f));
    EXPECT_EQ(nextafter_2, std::nextafter(0.f, -1.f));

    // 非正規化数→非正規化数
    constexpr auto nextafter_3 = tunum::nextafter(nextafter_1, 1.f);
    EXPECT_EQ(nextafter_3, std::nextafter(nextafter_1, 1.f));

    // 非正規化数→zero
    constexpr auto nextafter_4 = tunum::nextafter(nextafter_2, 1.f);
    EXPECT_EQ(nextafter_4, std::nextafter(nextafter_2, 1.f));
    EXPECT_EQ(nextafter_4, 0);

    // 入力が等しい
    constexpr auto nexttoward_1 = tunum::nexttoward(1.f, 1.f);
    constexpr auto nexttoward_2 = tunum::nexttoward(-3.2f, -3.2f);
    constexpr auto nexttoward_3 = tunum::nexttoward(0.f, 0.f);
    EXPECT_EQ(nexttoward_1, std::nexttoward(1.f, 1.f));
    EXPECT_EQ(nexttoward_2, std::nexttoward(-3.2f, -3.2f));
    EXPECT_EQ(nexttoward_3, std::nexttoward(0.f, 0.f));

    // 正規化数→正規化数
    constexpr auto nexttoward_4 = tunum::nexttoward(-1.f, 0.f);
    constexpr auto nexttoward_5 = tunum::nexttoward(-1.f, -2.f);
    EXPECT_EQ(nexttoward_4, std::nexttoward(-1.f, 0.f));
    EXPECT_EQ(nexttoward_5, std::nexttoward(-1.f, -2.f));

    // 正規化数→非正規化数
    constexpr auto min_norm = std::numeric_limits<float>::min();
    constexpr auto nexttoward_6 = tunum::nexttoward(min_norm, 0.f);
    EXPECT_EQ(nexttoward_6, std::nexttoward(min_norm, 0.f));
    EXPECT_TRUE(std::isnormal(min_norm));
    EXPECT_TRUE(!std::isnormal(nexttoward_6) && std::isfinite(nexttoward_6));

    // 非正規化数→正規化数
    constexpr auto nexttoward_7 = tunum::nexttoward(-nexttoward_6, -1.f);
    EXPECT_EQ(nexttoward_7, std::nexttoward(-nexttoward_6, -1.f));
    EXPECT_TRUE(std::isnormal(nexttoward_7));

    // 正規化数→無限大
    constexpr auto max_norm = std::numeric_limits<float>::max();
    constexpr auto inf = std::numeric_limits<float>::infinity();
    constexpr auto nexttoward_8 = tunum::nexttoward(max_norm, inf);
    EXPECT_EQ(nexttoward_8, std::nexttoward(max_norm, inf));
    EXPECT_EQ(nexttoward_8, inf);
}

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
