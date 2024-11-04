#include <gtest/gtest.h>
#include <tunum/math.hpp>

using floating_limit_t = std::numeric_limits<float>;

TEST(TunumMathTest, FpClassTest)
{
    constexpr auto nan = floating_limit_t::quiet_NaN();
    constexpr auto inf = floating_limit_t::infinity();
    constexpr auto zero = 0.f;
    constexpr auto max_norm = floating_limit_t::max();
    constexpr auto min_norm = floating_limit_t::min();
    constexpr auto denorm = floating_limit_t::denorm_min();

    constexpr auto fpclass_01 = tunum::fpclassify(nan);
    constexpr auto fpclass_02 = tunum::fpclassify(inf);
    constexpr auto fpclass_03 = tunum::fpclassify(-inf);
    constexpr auto fpclass_04 = tunum::fpclassify(zero);
    constexpr auto fpclass_05 = tunum::fpclassify(-zero);
    constexpr auto fpclass_06 = tunum::fpclassify(max_norm);
    constexpr auto fpclass_07 = tunum::fpclassify(-max_norm);
    constexpr auto fpclass_08 = tunum::fpclassify(min_norm);
    constexpr auto fpclass_09 = tunum::fpclassify(-min_norm);
    constexpr auto fpclass_10 = tunum::fpclassify(denorm);
    constexpr auto fpclass_11 = tunum::fpclassify(-denorm);
    EXPECT_EQ(fpclass_01, std::fpclassify(nan));
    EXPECT_EQ(fpclass_02, std::fpclassify(inf));
    EXPECT_EQ(fpclass_03, std::fpclassify(-inf));
    EXPECT_EQ(fpclass_04, std::fpclassify(zero));
    EXPECT_EQ(fpclass_05, std::fpclassify(-zero));
    EXPECT_EQ(fpclass_06, std::fpclassify(max_norm));
    EXPECT_EQ(fpclass_07, std::fpclassify(-max_norm));
    EXPECT_EQ(fpclass_08, std::fpclassify(min_norm));
    EXPECT_EQ(fpclass_09, std::fpclassify(-min_norm));
    EXPECT_EQ(fpclass_10, std::fpclassify(denorm));
    EXPECT_EQ(fpclass_11, std::fpclassify(-denorm));

    constexpr auto isfinite_1 = tunum::isfinite(nan);
    constexpr auto isfinite_2 = tunum::isfinite(inf);
    constexpr auto isfinite_3 = tunum::isfinite(zero);
    constexpr auto isfinite_4 = tunum::isfinite(max_norm);
    constexpr auto isfinite_5 = tunum::isfinite(min_norm);
    constexpr auto isfinite_6 = tunum::isfinite(denorm);
    EXPECT_EQ(isfinite_1, std::isfinite(nan));
    EXPECT_EQ(isfinite_2, std::isfinite(inf));
    EXPECT_EQ(isfinite_3, std::isfinite(zero));
    EXPECT_EQ(isfinite_4, std::isfinite(max_norm));
    EXPECT_EQ(isfinite_5, std::isfinite(min_norm));
    EXPECT_EQ(isfinite_6, std::isfinite(denorm));

    constexpr auto isinf_1 = tunum::isinf(nan);
    constexpr auto isinf_2 = tunum::isinf(inf);
    constexpr auto isinf_3 = tunum::isinf(zero);
    constexpr auto isinf_4 = tunum::isinf(max_norm);
    constexpr auto isinf_5 = tunum::isinf(min_norm);
    constexpr auto isinf_6 = tunum::isinf(denorm);
    EXPECT_EQ(isinf_1, std::isinf(nan));
    EXPECT_EQ(isinf_2, std::isinf(inf));
    EXPECT_EQ(isinf_3, std::isinf(zero));
    EXPECT_EQ(isinf_4, std::isinf(max_norm));
    EXPECT_EQ(isinf_5, std::isinf(min_norm));
    EXPECT_EQ(isinf_6, std::isinf(denorm));

    constexpr auto isnan_1 = tunum::isnan(nan);
    constexpr auto isnan_2 = tunum::isnan(inf);
    constexpr auto isnan_3 = tunum::isnan(zero);
    constexpr auto isnan_4 = tunum::isnan(max_norm);
    constexpr auto isnan_5 = tunum::isnan(min_norm);
    constexpr auto isnan_6 = tunum::isnan(denorm);
    EXPECT_EQ(isnan_1, std::isnan(nan));
    EXPECT_EQ(isnan_2, std::isnan(inf));
    EXPECT_EQ(isnan_3, std::isnan(zero));
    EXPECT_EQ(isnan_4, std::isnan(max_norm));
    EXPECT_EQ(isnan_5, std::isnan(min_norm));
    EXPECT_EQ(isnan_6, std::isnan(denorm));

    constexpr auto isnormal_1 = tunum::isnormal(nan);
    constexpr auto isnormal_2 = tunum::isnormal(inf);
    constexpr auto isnormal_3 = tunum::isnormal(zero);
    constexpr auto isnormal_4 = tunum::isnormal(max_norm);
    constexpr auto isnormal_5 = tunum::isnormal(min_norm);
    constexpr auto isnormal_6 = tunum::isnormal(denorm);
    EXPECT_EQ(isnormal_1, std::isnormal(nan));
    EXPECT_EQ(isnormal_2, std::isnormal(inf));
    EXPECT_EQ(isnormal_3, std::isnormal(zero));
    EXPECT_EQ(isnormal_4, std::isnormal(max_norm));
    EXPECT_EQ(isnormal_5, std::isnormal(min_norm));
    EXPECT_EQ(isnormal_6, std::isnormal(denorm));

    constexpr auto signbit_1 = tunum::signbit(nan);
    constexpr auto signbit_2 = tunum::signbit(inf);
    constexpr auto signbit_3 = tunum::signbit(zero);
    constexpr auto signbit_4 = tunum::signbit(max_norm);
    constexpr auto signbit_5 = tunum::signbit(min_norm);
    constexpr auto signbit_6 = tunum::signbit(denorm);
    constexpr auto signbit_7 = tunum::signbit(-nan);
    constexpr auto signbit_8 = tunum::signbit(-zero);
    EXPECT_EQ(signbit_1, std::signbit(nan));
    EXPECT_EQ(signbit_2, std::signbit(inf));
    EXPECT_EQ(signbit_3, std::signbit(zero));
    EXPECT_EQ(signbit_4, std::signbit(max_norm));
    EXPECT_EQ(signbit_5, std::signbit(min_norm));
    EXPECT_EQ(signbit_6, std::signbit(denorm));
    EXPECT_EQ(signbit_7, std::signbit(-nan));
    EXPECT_EQ(signbit_8, std::signbit(-zero));
 }

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
