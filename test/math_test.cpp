#include <gtest/gtest.h>
#include <tunum/math.hpp>

using floating_limit_t = std::numeric_limits<float>;
namespace test_values
{
    constexpr auto nan = floating_limit_t::quiet_NaN();
    constexpr auto inf = floating_limit_t::infinity();
    constexpr auto zero = 0.f;
    constexpr auto max_norm = floating_limit_t::max();
    constexpr auto min_norm = floating_limit_t::min();
    constexpr auto denorm = floating_limit_t::denorm_min();
}

TEST(TunumMathTest, FpClassTest)
{
    constexpr auto fpclass_01 = tunum::fpclassify(test_values::nan);
    constexpr auto fpclass_02 = tunum::fpclassify(test_values::inf);
    constexpr auto fpclass_03 = tunum::fpclassify(-test_values::inf);
    constexpr auto fpclass_04 = tunum::fpclassify(test_values::zero);
    constexpr auto fpclass_05 = tunum::fpclassify(-test_values::zero);
    constexpr auto fpclass_06 = tunum::fpclassify(test_values::max_norm);
    constexpr auto fpclass_07 = tunum::fpclassify(-test_values::max_norm);
    constexpr auto fpclass_08 = tunum::fpclassify(test_values::min_norm);
    constexpr auto fpclass_09 = tunum::fpclassify(-test_values::min_norm);
    constexpr auto fpclass_10 = tunum::fpclassify(test_values::denorm);
    constexpr auto fpclass_11 = tunum::fpclassify(-test_values::denorm);
    constexpr auto fpclass_12 = tunum::fpclassify(tunum::fe_holder{-test_values::denorm});
    EXPECT_EQ(fpclass_01, std::fpclassify(test_values::nan));
    EXPECT_EQ(fpclass_02, std::fpclassify(test_values::inf));
    EXPECT_EQ(fpclass_03, std::fpclassify(-test_values::inf));
    EXPECT_EQ(fpclass_04, std::fpclassify(test_values::zero));
    EXPECT_EQ(fpclass_05, std::fpclassify(-test_values::zero));
    EXPECT_EQ(fpclass_06, std::fpclassify(test_values::max_norm));
    EXPECT_EQ(fpclass_07, std::fpclassify(-test_values::max_norm));
    EXPECT_EQ(fpclass_08, std::fpclassify(test_values::min_norm));
    EXPECT_EQ(fpclass_09, std::fpclassify(-test_values::min_norm));
    EXPECT_EQ(fpclass_10, std::fpclassify(test_values::denorm));
    EXPECT_EQ(fpclass_11, std::fpclassify(-test_values::denorm));
    EXPECT_EQ(fpclass_12, std::fpclassify(-test_values::denorm));

    constexpr auto isfinite_1 = tunum::isfinite(test_values::nan);
    constexpr auto isfinite_2 = tunum::isfinite(test_values::inf);
    constexpr auto isfinite_3 = tunum::isfinite(test_values::zero);
    constexpr auto isfinite_4 = tunum::isfinite(test_values::max_norm);
    constexpr auto isfinite_5 = tunum::isfinite(test_values::min_norm);
    constexpr auto isfinite_6 = tunum::isfinite(test_values::denorm);
    constexpr auto isfinite_7 = tunum::isfinite(tunum::fe_holder{test_values::denorm});
    EXPECT_EQ(isfinite_1, std::isfinite(test_values::nan));
    EXPECT_EQ(isfinite_2, std::isfinite(test_values::inf));
    EXPECT_EQ(isfinite_3, std::isfinite(test_values::zero));
    EXPECT_EQ(isfinite_4, std::isfinite(test_values::max_norm));
    EXPECT_EQ(isfinite_5, std::isfinite(test_values::min_norm));
    EXPECT_EQ(isfinite_6, std::isfinite(test_values::denorm));
    EXPECT_EQ(isfinite_7, std::isfinite(test_values::denorm));

    constexpr auto isinf_1 = tunum::isinf(test_values::nan);
    constexpr auto isinf_2 = tunum::isinf(test_values::inf);
    constexpr auto isinf_3 = tunum::isinf(test_values::zero);
    constexpr auto isinf_4 = tunum::isinf(test_values::max_norm);
    constexpr auto isinf_5 = tunum::isinf(test_values::min_norm);
    constexpr auto isinf_6 = tunum::isinf(test_values::denorm);
    constexpr auto isinf_7 = tunum::isinf(tunum::fe_holder{test_values::denorm});
    EXPECT_EQ(isinf_1, std::isinf(test_values::nan));
    EXPECT_EQ(isinf_2, std::isinf(test_values::inf));
    EXPECT_EQ(isinf_3, std::isinf(test_values::zero));
    EXPECT_EQ(isinf_4, std::isinf(test_values::max_norm));
    EXPECT_EQ(isinf_5, std::isinf(test_values::min_norm));
    EXPECT_EQ(isinf_6, std::isinf(test_values::denorm));
    EXPECT_EQ(isinf_7, std::isinf(test_values::denorm));

    constexpr auto isnan_1 = tunum::isnan(test_values::nan);
    constexpr auto isnan_2 = tunum::isnan(test_values::inf);
    constexpr auto isnan_3 = tunum::isnan(test_values::zero);
    constexpr auto isnan_4 = tunum::isnan(test_values::max_norm);
    constexpr auto isnan_5 = tunum::isnan(test_values::min_norm);
    constexpr auto isnan_6 = tunum::isnan(test_values::denorm);
    constexpr auto isnan_7 = tunum::isnan(tunum::fe_holder{test_values::denorm});
    EXPECT_EQ(isnan_1, std::isnan(test_values::nan));
    EXPECT_EQ(isnan_2, std::isnan(test_values::inf));
    EXPECT_EQ(isnan_3, std::isnan(test_values::zero));
    EXPECT_EQ(isnan_4, std::isnan(test_values::max_norm));
    EXPECT_EQ(isnan_5, std::isnan(test_values::min_norm));
    EXPECT_EQ(isnan_6, std::isnan(test_values::denorm));
    EXPECT_EQ(isnan_7, std::isnan(test_values::denorm));

    constexpr auto isnormal_1 = tunum::isnormal(test_values::nan);
    constexpr auto isnormal_2 = tunum::isnormal(test_values::inf);
    constexpr auto isnormal_3 = tunum::isnormal(test_values::zero);
    constexpr auto isnormal_4 = tunum::isnormal(test_values::max_norm);
    constexpr auto isnormal_5 = tunum::isnormal(test_values::min_norm);
    constexpr auto isnormal_6 = tunum::isnormal(test_values::denorm);
    constexpr auto isnormal_7 = tunum::isnormal(tunum::fe_holder{test_values::denorm});
    EXPECT_EQ(isnormal_1, std::isnormal(test_values::nan));
    EXPECT_EQ(isnormal_2, std::isnormal(test_values::inf));
    EXPECT_EQ(isnormal_3, std::isnormal(test_values::zero));
    EXPECT_EQ(isnormal_4, std::isnormal(test_values::max_norm));
    EXPECT_EQ(isnormal_5, std::isnormal(test_values::min_norm));
    EXPECT_EQ(isnormal_6, std::isnormal(test_values::denorm));
    EXPECT_EQ(isnormal_7, std::isnormal(test_values::denorm));

    constexpr auto signbit_1 = tunum::signbit(test_values::nan);
    constexpr auto signbit_2 = tunum::signbit(test_values::inf);
    constexpr auto signbit_3 = tunum::signbit(test_values::zero);
    constexpr auto signbit_4 = tunum::signbit(test_values::max_norm);
    constexpr auto signbit_5 = tunum::signbit(test_values::min_norm);
    constexpr auto signbit_6 = tunum::signbit(test_values::denorm);
    constexpr auto signbit_7 = tunum::signbit(-test_values::nan);
    constexpr auto signbit_8 = tunum::signbit(-test_values::zero);
    constexpr auto signbit_9 = tunum::signbit(tunum::fe_holder{-test_values::zero});
    EXPECT_EQ(signbit_1, std::signbit(test_values::nan));
    EXPECT_EQ(signbit_2, std::signbit(test_values::inf));
    EXPECT_EQ(signbit_3, std::signbit(test_values::zero));
    EXPECT_EQ(signbit_4, std::signbit(test_values::max_norm));
    EXPECT_EQ(signbit_5, std::signbit(test_values::min_norm));
    EXPECT_EQ(signbit_6, std::signbit(test_values::denorm));
    EXPECT_EQ(signbit_7, std::signbit(-test_values::nan));
    EXPECT_EQ(signbit_8, std::signbit(-test_values::zero));
    EXPECT_EQ(signbit_9, std::signbit(-test_values::zero));
}

TEST(TunumMathTest, CopySignTest)
{
    constexpr auto copysign_1 = tunum::copysign(1, -2);
    constexpr auto copysign_2 = tunum::copysign(-1., 2.);
    constexpr auto copysign_3 = tunum::copysign(1.f, 2.f);
    constexpr auto copysign_4 = tunum::copysign(tunum::fe_holder{-1., FE_INEXACT}, 2.);
    constexpr auto copysign_5 = tunum::copysign(-1., tunum::fe_holder{2.});
    EXPECT_EQ(copysign_1, std::copysign(1, -2));
    EXPECT_EQ(copysign_2, std::copysign(-1., 2.));
    EXPECT_EQ(copysign_3, std::copysign(1.f, 2.f));
    EXPECT_EQ(copysign_4, std::copysign(-1., 2.));
    EXPECT_TRUE(copysign_4.has_inexact());
    EXPECT_EQ(copysign_5, std::copysign(-1., 2.));
}

TEST(TunumMathTest, NextAfterTest)
{
    // zero→非正規化数の計算
    constexpr auto nextafter_1 = tunum::nextafter(0.f, 1.f);
    constexpr auto nextafter_2 = tunum::nextafter(tunum::fe_holder{0.f}, -1.f);
    EXPECT_EQ(nextafter_1, std::nextafter(0.f, 1.f));
    EXPECT_EQ(nextafter_2, std::nextafter(0.f, -1.f));
    EXPECT_TRUE(nextafter_2.has_underflow());

    // 非正規化数→非正規化数
    constexpr auto nextafter_3 = tunum::nextafter(nextafter_1, tunum::fe_holder{1.f});
    EXPECT_EQ(nextafter_3, std::nextafter(nextafter_1, 1.f));
    EXPECT_TRUE(nextafter_3.has_underflow());

    // 非正規化数→zero
    constexpr auto nextafter_4 = tunum::nextafter(nextafter_2, tunum::fe_holder{1.f});
    EXPECT_EQ(nextafter_4, std::nextafter(nextafter_2, 1.f));
    EXPECT_EQ(nextafter_4, 0);
    EXPECT_TRUE(nextafter_4.has_underflow());

    // 入力が等しい
    constexpr auto nexttoward_1 = tunum::nexttoward(1.f, 1.f);
    constexpr auto nexttoward_2 = tunum::nexttoward(-3.2f, -3.2f);
    constexpr auto nexttoward_3 = tunum::nexttoward(tunum::fe_holder{0.f}, 0.f);
    constexpr auto nexttoward_inf_inf = tunum::nexttoward(tunum::fe_holder{test_values::inf}, test_values::inf);
    EXPECT_EQ(nexttoward_1, std::nexttoward(1.f, 1.f));
    EXPECT_EQ(nexttoward_2, std::nexttoward(-3.2f, -3.2f));
    EXPECT_EQ(nexttoward_3, std::nexttoward(0.f, 0.f));
    EXPECT_EQ(nexttoward_inf_inf, test_values::inf);
    EXPECT_FALSE(nexttoward_3.has_fexcept());
    EXPECT_FALSE(nexttoward_inf_inf.has_fexcept());

    // 正規化数→正規化数
    constexpr auto nexttoward_4 = tunum::nexttoward(-1.f, 0.f);
    constexpr auto nexttoward_5 = tunum::nexttoward(tunum::fe_holder{-1.f}, -2.f);
    EXPECT_EQ(nexttoward_4, std::nexttoward(-1.f, 0.f));
    EXPECT_EQ(nexttoward_5, std::nexttoward(-1.f, -2.f));
    EXPECT_FALSE(nexttoward_5.has_fexcept());

    // 正規化数→非正規化数
    constexpr auto min_norm = std::numeric_limits<float>::min();
    constexpr auto nexttoward_6 = tunum::nexttoward(min_norm, tunum::fe_holder{0.f});
    EXPECT_EQ(nexttoward_6, std::nexttoward(min_norm, 0.f));
    EXPECT_TRUE(std::isnormal(test_values::min_norm));
    EXPECT_TRUE(!std::isnormal(nexttoward_6) && std::isfinite(nexttoward_6));
    EXPECT_TRUE(nexttoward_6.has_underflow());

    // 非正規化数→正規化数
    constexpr auto nexttoward_7 = tunum::nexttoward(-nexttoward_6.value, tunum::fe_holder{-1.f});
    EXPECT_EQ(nexttoward_7, std::nexttoward(-nexttoward_6, -1.f));
    EXPECT_TRUE(std::isnormal(nexttoward_7));
    EXPECT_FALSE(nexttoward_7.has_fexcept());

    // 正規化数→無限大
    constexpr auto max_norm = std::numeric_limits<float>::max();
    constexpr auto inf = std::numeric_limits<float>::infinity();
    constexpr auto nexttoward_8 = tunum::nexttoward(max_norm, tunum::fe_holder{inf});
    EXPECT_EQ(nexttoward_8, std::nexttoward(max_norm, inf));
    EXPECT_EQ(nexttoward_8, inf);
    EXPECT_TRUE(nexttoward_8.has_overflow());

    // 無限大を小さいほうに動かそうとしたときの動作
    constexpr auto inf_to_under1 = tunum::nexttoward(inf, tunum::fe_holder{max_norm});
    constexpr auto inf_to_under2 = tunum::nexttoward(inf, tunum::fe_holder{0.f});
    constexpr auto inf_to_under3 = tunum::nexttoward(inf, tunum::fe_holder{-inf});
    EXPECT_EQ(inf_to_under1, std::nexttoward(inf, max_norm));
    EXPECT_EQ(inf_to_under2, std::nexttoward(inf, 0.f));
    EXPECT_EQ(inf_to_under3, std::nexttoward(inf, -inf));
    EXPECT_FALSE(inf_to_under1.has_fexcept());
    EXPECT_FALSE(inf_to_under2.has_fexcept());
    EXPECT_FALSE(inf_to_under3.has_fexcept());

    // 整数 -> 整数
    constexpr auto nextafter_i = tunum::nextafter(1, 2);
    constexpr auto nexttoward_i = tunum::nextafter(-1, -2);
    EXPECT_EQ(nextafter_i, std::nextafter(1., 2.));
    EXPECT_EQ(nexttoward_i, std::nexttoward(-1., -2.));

    // 整数を絡めた異なる型同士
    constexpr auto nextafter_an1 = tunum::nextafter(0.f, 5);
    constexpr auto nextafter_an2 = tunum::nextafter(tunum::fe_holder{0.f}, 5);
    constexpr auto nexttoward_an1 = tunum::nexttoward(0, 5.f);
    constexpr auto nexttoward_an2 = tunum::nexttoward(0ul, tunum::fe_holder{5.f});
    EXPECT_EQ(nextafter_an1, std::nextafter(0.f, 5));
    EXPECT_EQ(nextafter_an2, std::nextafter(0.f, 5));
    EXPECT_EQ(nexttoward_an1, std::nextafter(0, 5.f));
    EXPECT_EQ(nexttoward_an2, std::nextafter(0ul, 5.f));
}

TEST(TunumMathTest, NearIntegralTest)
{
    constexpr auto ceil_01 = tunum::ceil(2.f);
    constexpr auto ceil_02 = tunum::ceil(-2.f);
    constexpr auto ceil_03 = tunum::ceil(2.1f);
    constexpr auto ceil_04 = tunum::ceil(-2.1f);
    constexpr auto ceil_05 = tunum::ceil(test_values::denorm);
    constexpr auto ceil_06 = tunum::ceil(-test_values::denorm);
    constexpr auto ceil_07 = tunum::ceil(test_values::zero);
    constexpr auto ceil_08 = tunum::ceil(-test_values::zero);
    constexpr auto ceil_09 = tunum::ceil(test_values::inf);
    constexpr auto ceil_10 = tunum::ceil(-test_values::inf);
    constexpr auto ceil_11 = tunum::ceil(2);
    constexpr auto ceil_12 = tunum::ceil(tunum::fe_holder{2.f});
    EXPECT_EQ(ceil_01, std::ceil(2.f));
    EXPECT_EQ(ceil_02, std::ceil(-2.f));
    EXPECT_EQ(ceil_03, std::ceil(2.1f));
    EXPECT_EQ(ceil_04, std::ceil(-2.1f));
    EXPECT_EQ(ceil_05, std::ceil(test_values::denorm));
    EXPECT_EQ(ceil_06, std::ceil(-test_values::denorm));
    EXPECT_EQ(ceil_07, std::ceil(test_values::zero));
    EXPECT_EQ(ceil_08, std::ceil(-test_values::zero));
    EXPECT_EQ(ceil_09, std::ceil(test_values::inf));
    EXPECT_EQ(ceil_10, std::ceil(-test_values::inf));
    EXPECT_EQ(ceil_11, std::ceil(2));
    EXPECT_EQ(ceil_12, std::ceil(2.f));

    constexpr auto floor_01 = tunum::floor(2.f);
    constexpr auto floor_02 = tunum::floor(-2.f);
    constexpr auto floor_03 = tunum::floor(2.1f);
    constexpr auto floor_04 = tunum::floor(-2.1f);
    constexpr auto floor_05 = tunum::floor(test_values::denorm);
    constexpr auto floor_06 = tunum::floor(-test_values::denorm);
    constexpr auto floor_07 = tunum::floor(test_values::zero);
    constexpr auto floor_08 = tunum::floor(-test_values::zero);
    constexpr auto floor_09 = tunum::floor(test_values::inf);
    constexpr auto floor_10 = tunum::floor(-test_values::inf);
    constexpr auto floor_11 = tunum::floor(2);
    constexpr auto floor_12 = tunum::floor(tunum::fe_holder{2.f});
    EXPECT_EQ(floor_01, std::floor(2.f));
    EXPECT_EQ(floor_02, std::floor(-2.f));
    EXPECT_EQ(floor_03, std::floor(2.1f));
    EXPECT_EQ(floor_04, std::floor(-2.1f));
    EXPECT_EQ(floor_05, std::floor(test_values::denorm));
    EXPECT_EQ(floor_06, std::floor(-test_values::denorm));
    EXPECT_EQ(floor_07, std::floor(test_values::zero));
    EXPECT_EQ(floor_08, std::floor(-test_values::zero));
    EXPECT_EQ(floor_09, std::floor(test_values::inf));
    EXPECT_EQ(floor_10, std::floor(-test_values::inf));
    EXPECT_EQ(floor_11, std::floor(2));
    EXPECT_EQ(floor_12, std::floor(2.f));

    constexpr auto trunc_01 = tunum::trunc(2.f);
    constexpr auto trunc_02 = tunum::trunc(-2.f);
    constexpr auto trunc_03 = tunum::trunc(2.9f);
    constexpr auto trunc_04 = tunum::trunc(-2.9f);
    constexpr auto trunc_05 = tunum::trunc(test_values::denorm);
    constexpr auto trunc_06 = tunum::trunc(-test_values::denorm);
    constexpr auto trunc_07 = tunum::trunc(test_values::zero);
    constexpr auto trunc_08 = tunum::trunc(-test_values::zero);
    constexpr auto trunc_09 = tunum::trunc(test_values::inf);
    constexpr auto trunc_10 = tunum::trunc(-test_values::inf);
    constexpr auto trunc_11 = tunum::trunc(2);
    constexpr auto trunc_12 = tunum::trunc(tunum::fe_holder{2.f});
    EXPECT_EQ(trunc_01, std::trunc(2.f));
    EXPECT_EQ(trunc_02, std::trunc(-2.f));
    EXPECT_EQ(trunc_03, std::trunc(2.9f));
    EXPECT_EQ(trunc_04, std::trunc(-2.9f));
    EXPECT_EQ(trunc_05, std::trunc(test_values::denorm));
    EXPECT_EQ(trunc_06, std::trunc(-test_values::denorm));
    EXPECT_EQ(trunc_07, std::trunc(test_values::zero));
    EXPECT_EQ(trunc_08, std::trunc(-test_values::zero));
    EXPECT_EQ(trunc_09, std::trunc(test_values::inf));
    EXPECT_EQ(trunc_10, std::trunc(-test_values::inf));
    EXPECT_EQ(trunc_11, std::trunc(2));
    EXPECT_EQ(trunc_12, std::trunc(2.f));

    constexpr auto round_01 = tunum::round(2.f);
    constexpr auto round_02 = tunum::round(-2.f);
    constexpr auto round_03 = tunum::round(2.4f);
    constexpr auto round_04 = tunum::round(-2.4f);
    constexpr auto round_05 = tunum::round(test_values::denorm);
    constexpr auto round_06 = tunum::round(-test_values::denorm);
    constexpr auto round_07 = tunum::round(test_values::zero);
    constexpr auto round_08 = tunum::round(-test_values::zero);
    constexpr auto round_09 = tunum::round(test_values::inf);
    constexpr auto round_10 = tunum::round(-test_values::inf);
    constexpr auto round_11 = tunum::round(2.5f);
    constexpr auto round_12 = tunum::round(-2.5f);
    constexpr auto round_13 = tunum::round(2);
    constexpr auto round_14 = tunum::round(tunum::fe_holder{-2.5f, FE_INEXACT});
    EXPECT_EQ(round_01, std::round(2.f));
    EXPECT_EQ(round_02, std::round(-2.f));
    EXPECT_EQ(round_03, std::round(2.4f));
    EXPECT_EQ(round_04, std::round(-2.4f));
    EXPECT_EQ(round_05, std::round(test_values::denorm));
    EXPECT_EQ(round_06, std::round(-test_values::denorm));
    EXPECT_EQ(round_07, std::round(test_values::zero));
    EXPECT_EQ(round_08, std::round(-test_values::zero));
    EXPECT_EQ(round_09, std::round(test_values::inf));
    EXPECT_EQ(round_10, std::round(-test_values::inf));
    EXPECT_EQ(round_11, std::round(2.5f));
    EXPECT_EQ(round_12, std::round(-2.5f));
    EXPECT_EQ(round_13, std::round(2));
    EXPECT_EQ(round_14, std::round(-2.5f));
    EXPECT_TRUE(round_14.has_inexact());
}

TEST(TunumMathTest, FractionExponentTest)
{
    constexpr auto std_modf_arr = [](float value) {
        auto integral_part = float{};
        return std::array<float, 2>{std::modf(value, &integral_part), integral_part};
    };

    constexpr auto modf_arr_01 = tunum::modf_arr(2.f);
    constexpr auto modf_arr_02 = tunum::modf_arr(-2.f);
    constexpr auto modf_arr_03 = tunum::modf_arr(2.4f);
    constexpr auto modf_arr_04 = tunum::modf_arr(-2.4f);
    constexpr auto modf_arr_05 = tunum::modf_arr(test_values::denorm);
    constexpr auto modf_arr_06 = tunum::modf_arr(-test_values::denorm);
    constexpr auto modf_arr_07 = tunum::modf_arr(test_values::zero);
    constexpr auto modf_arr_08 = tunum::modf_arr(-test_values::zero);
    constexpr auto modf_arr_09 = tunum::modf_arr(test_values::inf);
    constexpr auto modf_arr_10 = tunum::modf_arr(-test_values::inf);
    constexpr auto modf_arr_11 = tunum::modf_arr(test_values::nan);
    auto [fe_01, ie_01] = std_modf_arr(2.f);
    auto [fe_02, ie_02] = std_modf_arr(-2.f);
    auto [fe_03, ie_03] = std_modf_arr(2.4f);
    auto [fe_04, ie_04] = std_modf_arr(-2.4f);
    auto [fe_05, ie_05] = std_modf_arr(test_values::denorm);
    auto [fe_06, ie_06] = std_modf_arr(-test_values::denorm);
    auto [fe_07, ie_07] = std_modf_arr(test_values::zero);
    auto [fe_08, ie_08] = std_modf_arr(-test_values::zero);
    auto [fe_09, ie_09] = std_modf_arr(test_values::inf);
    auto [fe_10, ie_10] = std_modf_arr(-test_values::inf);
    auto [fe_11, ie_11] = std_modf_arr(test_values::nan);
    EXPECT_EQ(modf_arr_01[0], fe_01);
    EXPECT_EQ(modf_arr_02[0], fe_02);
    EXPECT_EQ(modf_arr_03[0], fe_03);
    EXPECT_EQ(modf_arr_04[0], fe_04);
    EXPECT_EQ(modf_arr_05[0], fe_05);
    EXPECT_EQ(modf_arr_06[0], fe_06);
    EXPECT_EQ(modf_arr_07[0], fe_07);
    EXPECT_EQ(modf_arr_08[0], fe_08);
    EXPECT_EQ(modf_arr_09[0], fe_09);
    EXPECT_EQ(modf_arr_10[0], fe_10);
    EXPECT_EQ(std::isnan(modf_arr_11[0]), std::isnan(fe_11));

    EXPECT_EQ(modf_arr_01[1], ie_01);
    EXPECT_EQ(modf_arr_02[1], ie_02);
    EXPECT_EQ(modf_arr_03[1], ie_03);
    EXPECT_EQ(modf_arr_04[1], ie_04);
    EXPECT_EQ(modf_arr_05[1], ie_05);
    EXPECT_EQ(modf_arr_06[1], ie_06);
    EXPECT_EQ(modf_arr_07[1], ie_07);
    EXPECT_EQ(modf_arr_08[1], ie_08);
    EXPECT_EQ(modf_arr_09[1], ie_09);
    EXPECT_EQ(modf_arr_10[1], ie_10);
    EXPECT_EQ(std::isnan(modf_arr_11[1]), std::isnan(ie_11));

    constexpr auto modf_arr_fe = tunum::modf_arr(tunum::fe_holder{2.4f, FE_INEXACT});
    EXPECT_EQ(modf_arr_fe[0], fe_03);
    EXPECT_EQ(modf_arr_fe[1], ie_03);
    EXPECT_TRUE(modf_arr_fe[0].has_inexact());
    EXPECT_TRUE(modf_arr_fe[1].has_inexact());

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
