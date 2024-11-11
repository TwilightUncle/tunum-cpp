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

    // 各種判定のテスト
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

    // 型キャストおよび、浮動小数点型取得メンバ関数テスト
    EXPECT_EQ(static_cast<float>(info11), float(11));
    EXPECT_EQ(info9.get_integral_part(), .0);
    EXPECT_EQ(info9.get_decimal_part(), .1);
    EXPECT_EQ(info10.get_integral_part(), double(-1));
    EXPECT_EQ(info10.get_decimal_part(), double(1. - 1.1));
    EXPECT_EQ(info11.get_integral_part(), float(11));
    EXPECT_EQ(info11.get_decimal_part(), .0);
    EXPECT_TRUE(tunum::floating_std_info{info3.get_integral_part()}.is_nan());
    EXPECT_TRUE(tunum::floating_std_info{info3.get_decimal_part()}.is_nan());
    EXPECT_EQ(info5.get_integral_part(), float_limit::infinity());
    EXPECT_EQ(info5.get_decimal_part(), 0.);
    EXPECT_EQ(info6.get_integral_part(), -double_limit::infinity());
    EXPECT_EQ(info6.get_decimal_part(), -0.);
    EXPECT_EQ(info7.get_integral_part(), 0.);
    EXPECT_EQ(info7.get_decimal_part(), double_limit::denorm_min());
    EXPECT_EQ(info8.get_integral_part(), -float(0.));
    EXPECT_EQ(info8.get_decimal_part(), -float_limit::denorm_min());

    // 生成系
    constexpr auto zero_info = info1.make_zero();
    constexpr auto inf_info = info1.make_infinity(true);
    constexpr auto denorm_min = info1.make_denormalized_min();
    constexpr auto denorm_max = info1.make_denormalized_max(true);
    constexpr auto norm_min = info1.make_min(true);
    constexpr auto norm_max = info1.make_max();
    EXPECT_EQ((double)zero_info, 0);
    EXPECT_EQ((double)inf_info, -double_limit::infinity());
    EXPECT_TRUE(denorm_min.is_denormalized());
    EXPECT_TRUE(denorm_max.is_denormalized());
    EXPECT_GT(-(double)denorm_max, (double)denorm_min);
    EXPECT_EQ(-(double)denorm_max + (double)denorm_min, -(double)norm_min);
    EXPECT_EQ((double)norm_min, -double_limit::min());
    EXPECT_EQ((double)norm_max, double_limit::max());

    // 整数乗専用のexp2
    EXPECT_EQ(info1.exp2_integral(0), 1 << 0);
    EXPECT_EQ(info1.exp2_integral(1), 1 << 1);
    EXPECT_EQ(info1.exp2_integral(2), 1 << 2);
    EXPECT_EQ(info1.exp2_integral(63), 1ull << 63);
    EXPECT_EQ(info1.exp2_integral(64), std::exp2(64.));
    EXPECT_EQ(info1.exp2_integral(-64), std::exp2(-64.));
    // オーバーフロー、アンダーフローの挙動チェック
    EXPECT_EQ(info1.exp2_integral(10000), std::exp2(10000.));
    EXPECT_EQ(info1.exp2_integral(-10000), std::exp2(-10000.));
}

TEST(TunumFloatingTest, FeHolderTest)
{
    using limit_t = std::numeric_limits<double>;
    constexpr auto default_info = tunum::floating_std_info{0.};

    // コンストラクタとか推論(型があってればOK)
    using fe_holder_val_t_1 = decltype(tunum::fe_holder{1.});
    using fe_holder_val_t_2 = decltype(tunum::fe_holder{1.f, FE_ALL_EXCEPT});
    using fe_holder_val_t_3 = decltype(tunum::fe_holder{1ull});
    using fe_holder_val_t_4 = decltype(tunum::fe_holder{1ll, FE_ALL_EXCEPT});
    using fe_holder_val_t_5 = decltype(tunum::fe_holder{tunum::fe_holder{1., FE_ALL_EXCEPT}});
    static_assert(std::is_same_v<fe_holder_val_t_1, tunum::fe_holder<double>>);
    static_assert(std::is_same_v<fe_holder_val_t_2, tunum::fe_holder<float>>);
    static_assert(std::is_same_v<fe_holder_val_t_3, tunum::fe_holder<double>>);
    static_assert(std::is_same_v<fe_holder_val_t_4, tunum::fe_holder<double>>);
    static_assert(std::is_same_v<fe_holder_val_t_5, tunum::fe_holder<double>>);

    // 浮動小数点例外保持型を浮動小数点型への変換を挟まずに解釈オブジェクトへの変換が可能か
    constexpr auto convert_info = tunum::floating_std_info{
        tunum::fe_holder{limit_t::infinity()}
    };
    EXPECT_TRUE(convert_info.is_infinity());

    // 実行時の浮動小数点例外検出確認
    if (!std::is_constant_evaluated()) {
        const auto check_exp_exception = tunum::fe_holder{[] { return std::exp(1000); }};
        EXPECT_TRUE(check_exp_exception.has_fexcept());
        EXPECT_TRUE(check_exp_exception.has_inexact());
        EXPECT_FALSE(check_exp_exception.has_divbyzero());
        EXPECT_FALSE(check_exp_exception.has_invalid());
        EXPECT_TRUE(check_exp_exception.has_overflow());
        EXPECT_FALSE(check_exp_exception.has_underflow());

        // システム変数のリセットや差戻周り
        std::fexcept_t bef = {};
        std::fegetexceptflag(&bef, FE_ALL_EXCEPT);
        const auto expect_non_exception = tunum::fe_holder{[] { return 1. + 1.; }};
        std::fexcept_t af = {};
        std::fegetexceptflag(&af, FE_ALL_EXCEPT);
        // fe_holderの内部でリセットと差戻を行っているが、その部分が正常か
        EXPECT_EQ(bef, af);
        // fe_holderのコンストラクタの計算は例外が発生していないこと
        EXPECT_FALSE(expect_non_exception.has_fexcept());
    }
}

TEST(TunumFloatingTest, AddTest)
{
    // 型推論と、結果が大きいほうの型に統一されているか
    constexpr auto add_1 = tunum::add(0.f, tunum::fe_holder{3.3});
    constexpr auto add_2 = tunum::add(tunum::fe_holder{0.3}, -7);
    constexpr auto add_3 = tunum::add(2, 8.3f);
    constexpr auto add_4 = tunum::add(-1, 5ull);
    static_assert(std::is_same_v<typename decltype(add_1)::calc_t, double>);
    static_assert(std::is_same_v<typename decltype(add_2)::calc_t, double>);
    static_assert(std::is_same_v<typename decltype(add_3)::calc_t, float>);
    static_assert(std::is_same_v<typename decltype(add_4)::calc_t, float>);
    EXPECT_EQ((double)add_1, 0.f + 3.3);
    EXPECT_EQ((double)add_2, 0.3 + -7);
    EXPECT_EQ((float)add_3, 2 + 8.3f);
    EXPECT_EQ((float)add_4, 4.f);
}

TEST(TunumFloatingTest, FeHolderOperatorTest)
{
    // 比較
    // 単項+-

    // 下記算術演算子の細かいテストは専用のテスト関数で行う
    // ここでは、正常な呼び出しの確認が取れればOK

    // 加算

    // 減算

    // 乗算

    // 除算

}
