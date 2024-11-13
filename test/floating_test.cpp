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
    constexpr auto nan_info = info1.make_nan();
    EXPECT_EQ((double)zero_info, 0);
    EXPECT_EQ((double)inf_info, -double_limit::infinity());
    EXPECT_TRUE(denorm_min.is_denormalized());
    EXPECT_TRUE(denorm_max.is_denormalized());
    EXPECT_GT(-(double)denorm_max, (double)denorm_min);
    EXPECT_EQ(-(double)denorm_max + (double)denorm_min, -(double)norm_min);
    EXPECT_EQ((double)norm_min, -double_limit::min());
    EXPECT_EQ((double)norm_max, double_limit::max());
    EXPECT_TRUE(nan_info.is_nan());

    // 整数乗専用のexp2
    EXPECT_EQ(info1.exp2_integral(0), 1 << 0);
    EXPECT_EQ(info1.exp2_integral(1), 1 << 1);
    EXPECT_EQ(info1.exp2_integral(2), 1 << 2);
    EXPECT_EQ(info1.exp2_integral(63), 1ull << 63);
    EXPECT_EQ(info1.exp2_integral(64), std::exp2(64.));
    EXPECT_EQ(info1.exp2_integral(-64), std::exp2(-64.));
    // オーバーフロー、アンダーフローの挙動チェック
    EXPECT_EQ(info1.exp2_integral(10000), std::exp2(10000.));
    // -1086と-1087が不具合の境界(過去)
    EXPECT_EQ(info1.exp2_integral(-1087), std::exp2(-1087.));
    EXPECT_EQ(info1.exp2_integral(-10000), std::exp2(-10000.));

    // 2のn乗についての境界検査
    constexpr auto norm_min2 = info1.make_min();
    // 正規化数最小値を右1シフト
    constexpr auto border_shift_r_1 = norm_min2.add_exponent(-1);
    // 非正規化数最小値を右1シフト
    constexpr auto border_shift_r_2 = denorm_min.add_exponent(-1);
    EXPECT_EQ((double)border_shift_r_1, (double)norm_min2 / 2);
    EXPECT_EQ((double)border_shift_r_2, (double)denorm_min / 2);

    // 情報落ちを発生させる/させない情報を正確に取れているか
    constexpr auto half_norm_max_info = tunum::floating_std_info{(double)norm_max / 2};
    constexpr auto lost_info = half_norm_max_info.make_lost_info_max();
    constexpr auto not_lost_info = half_norm_max_info.make_non_lost_info_min();
    EXPECT_EQ((double)half_norm_max_info + (double)lost_info, (double)half_norm_max_info);
    EXPECT_GT((double)half_norm_max_info + (double)not_lost_info, (double)half_norm_max_info);
    EXPECT_TRUE(tunum::floating_std_info{(double)half_norm_max_info + (double)not_lost_info}.is_normalized());
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

TEST(TunumFloatingTest, FeHolderOperatorTest)
{
    constexpr auto float_info = tunum::floating_std_info{0.f};

    // 算術以外の演算子オーバーロードテスト
    // 内部の浮動小数点型をそのまま比較しているだけなので、型キャストメインで確認
    constexpr auto val1 = tunum::fe_holder{1.5};
    constexpr auto val2 = -val1;
    EXPECT_TRUE(val1 > val2);
    EXPECT_TRUE(val1 <= tunum::fe_holder{1.5f});
    EXPECT_TRUE(val1 <= tunum::fe_holder{1.6f});
    EXPECT_TRUE(tunum::fe_holder{-1.9f} < val2);
    EXPECT_TRUE(-1.5f >= val2);
    EXPECT_TRUE(-1.5 == val2);
    EXPECT_TRUE(val1 != -1.5f);

    constexpr auto val3 = tunum::fe_holder{0.};
    EXPECT_TRUE(static_cast<bool>(val1));
    EXPECT_FALSE(static_cast<bool>(val3));
    EXPECT_FALSE(!val2);
    EXPECT_TRUE(!val3);
}

TEST(TunumFloatingTest, AddTest)
{
    constexpr auto float_info = tunum::floating_std_info{0.f};

    // 型推論と、結果が大きいほうの型に統一されているか
    // 整数同士の計算はfloatになるか
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

    // 引数にNaN, 無限大が含まれた場合
    constexpr auto arg_nan1 = tunum::add(float_info.get_nan(), 1.f);
    EXPECT_TRUE(tunum::floating_std_info{arg_nan1}.is_nan());
    EXPECT_FALSE(arg_nan1.has_fexcept());
    constexpr auto arg_nan2 = tunum::add(1.f, float_info.get_nan(true));
    EXPECT_TRUE(tunum::floating_std_info{arg_nan2}.is_nan());
    EXPECT_FALSE(arg_nan2.has_fexcept());
    constexpr auto arg_nan3 = tunum::add(float_info.get_nan(), float_info.get_nan(true));
    EXPECT_TRUE(tunum::floating_std_info{arg_nan3}.is_nan());
    EXPECT_FALSE(arg_nan3.has_fexcept());

    constexpr auto arg_inf1 = tunum::add(float_info.get_infinity(), 1.f);
    EXPECT_TRUE(tunum::floating_std_info{arg_inf1}.is_infinity());
    EXPECT_TRUE(tunum::floating_std_info{arg_inf1}.sign() > 0);
    EXPECT_FALSE(arg_inf1.has_fexcept());
    // 正の正規化数最大値と負の無限大
    constexpr auto arg_inf2 = tunum::add(float_info.get_max(), float_info.get_infinity(true));
    EXPECT_TRUE(tunum::floating_std_info{arg_inf2}.is_infinity());
    EXPECT_TRUE(tunum::floating_std_info{arg_inf2}.sign() < 0);
    EXPECT_FALSE(arg_inf2.has_fexcept());
    constexpr auto arg_inf3 = tunum::add(float_info.get_infinity(), float_info.get_infinity());
    EXPECT_TRUE(tunum::floating_std_info{arg_inf3}.is_infinity());
    EXPECT_TRUE(tunum::floating_std_info{arg_inf3}.sign() > 0);
    EXPECT_FALSE(arg_inf3.has_fexcept());
    constexpr auto arg_inf4 = tunum::add(float_info.get_infinity(true), float_info.get_infinity(true));
    EXPECT_TRUE(tunum::floating_std_info{arg_inf4}.is_infinity());
    EXPECT_TRUE(tunum::floating_std_info{arg_inf4}.sign() < 0);
    EXPECT_FALSE(arg_inf4.has_fexcept());

    constexpr auto arg_nan_inf = tunum::add(float_info.get_nan(), float_info.get_infinity());
    EXPECT_TRUE(tunum::floating_std_info{arg_nan_inf}.is_nan());
    EXPECT_FALSE(arg_nan_inf.has_fexcept());
    constexpr auto arg_inf_nan = tunum::add(float_info.get_infinity(), float_info.get_nan());
    EXPECT_TRUE(tunum::floating_std_info{arg_inf_nan}.is_nan());
    EXPECT_FALSE(arg_inf_nan.has_fexcept());

    // 等しい値によるゼロでは例外発生なし
    constexpr auto same_arg = tunum::add(1.f, -1.f);
    EXPECT_EQ((float)same_arg, 0);
    EXPECT_FALSE(same_arg.has_fexcept());

    // 無限大 - 無限大
    constexpr auto inf_minus_inf1 = tunum::add(float_info.get_infinity(), float_info.get_infinity(true));
    EXPECT_TRUE(tunum::floating_std_info{inf_minus_inf1}.is_nan());
    EXPECT_FALSE(inf_minus_inf1.has_inexact());
    EXPECT_FALSE(inf_minus_inf1.has_divbyzero());
    EXPECT_TRUE(inf_minus_inf1.has_invalid());
    EXPECT_FALSE(inf_minus_inf1.has_overflow());
    EXPECT_FALSE(inf_minus_inf1.has_underflow());
    constexpr auto inf_minus_inf2 = tunum::add(float_info.get_infinity(true), float_info.get_infinity());
    EXPECT_TRUE(tunum::floating_std_info{inf_minus_inf2}.is_nan());
    EXPECT_FALSE(inf_minus_inf2.has_inexact());
    EXPECT_FALSE(inf_minus_inf2.has_divbyzero());
    EXPECT_TRUE(inf_minus_inf2.has_invalid());
    EXPECT_FALSE(inf_minus_inf2.has_overflow());
    EXPECT_FALSE(inf_minus_inf2.has_underflow());

    // アンダーフロー
    constexpr auto udf_1 = tunum::add(float_info.get_min(), float_info.get_denormalized_min(true));
    EXPECT_TRUE(tunum::floating_std_info{udf_1}.is_denormalized());
    EXPECT_TRUE((float)udf_1 > 0);
    EXPECT_TRUE(udf_1.has_inexact());
    EXPECT_FALSE(udf_1.has_divbyzero());
    EXPECT_FALSE(udf_1.has_invalid());
    EXPECT_FALSE(udf_1.has_overflow());
    EXPECT_TRUE(udf_1.has_underflow());
    constexpr auto udf_2 = tunum::add(float_info.get_denormalized_max(true), float_info.get_denormalized_min());
    EXPECT_TRUE(tunum::floating_std_info{udf_2}.is_denormalized());
    EXPECT_TRUE((float)udf_2 < 0);
    EXPECT_TRUE(udf_2.has_inexact());
    EXPECT_FALSE(udf_2.has_divbyzero());
    EXPECT_FALSE(udf_2.has_invalid());
    EXPECT_FALSE(udf_2.has_overflow());
    EXPECT_TRUE(udf_2.has_underflow());

    // オーバーフロー(本来有限数になるはずだが、型で表現可能な値を逸脱することによる無限大発生)
    // もうちょい境界チェックしたほうが良いかもだがめんどいのでパス。
    constexpr auto ovf_1 = tunum::add(float_info.get_max(), float_info.get_max());
    EXPECT_TRUE(tunum::floating_std_info{ovf_1}.is_infinity());
    EXPECT_TRUE((float)ovf_1 > 0);
    EXPECT_TRUE(ovf_1.has_inexact());
    EXPECT_FALSE(ovf_1.has_divbyzero());
    EXPECT_FALSE(ovf_1.has_invalid());
    EXPECT_TRUE(ovf_1.has_overflow());
    EXPECT_FALSE(ovf_1.has_underflow());
    constexpr auto ovf_2 = tunum::add(float_info.get_max(true), float_info.get_max(true));
    EXPECT_TRUE(tunum::floating_std_info{ovf_2}.is_infinity());
    EXPECT_TRUE((float)ovf_2 < 0);
    EXPECT_TRUE(ovf_2.has_inexact());
    EXPECT_FALSE(ovf_2.has_divbyzero());
    EXPECT_FALSE(ovf_2.has_invalid());
    EXPECT_TRUE(ovf_2.has_overflow());
    EXPECT_FALSE(ovf_2.has_underflow());
}

TEST(TunumFloatingTest, SubTest)
{
    // 加算にて大体の確認はしているため、推論の確認と軽い動作確認
    // 型推論と、結果が大きいほうの型に統一されているか
    // 整数同士の計算はfloatになるか
    constexpr auto sub_1 = tunum::sub(0.f, tunum::fe_holder{3.3});
    constexpr auto sub_2 = tunum::sub(tunum::fe_holder{0.3}, -7);
    constexpr auto sub_3 = tunum::sub(2, 8.3f);
    constexpr auto sub_4 = tunum::sub(-1, 5ull);
    static_assert(std::is_same_v<typename decltype(sub_1)::calc_t, double>);
    static_assert(std::is_same_v<typename decltype(sub_2)::calc_t, double>);
    static_assert(std::is_same_v<typename decltype(sub_3)::calc_t, float>);
    static_assert(std::is_same_v<typename decltype(sub_4)::calc_t, float>);
    EXPECT_EQ((double)sub_1, 0.f - 3.3);
    EXPECT_EQ((double)sub_2, 0.3 - -7);
    EXPECT_EQ((float)sub_3, 2 - 8.3f);
    EXPECT_EQ((float)sub_4, -6.f);

}

TEST(TunumFloatingTest, MulTest)
{
    constexpr auto float_info = tunum::floating_std_info{0.f};

    // 型推論と、結果が大きいほうの型に統一されているか
    // 整数同士の計算はfloatになるか
    constexpr auto mul_1 = tunum::mul(0.f, tunum::fe_holder{3.3});
    constexpr auto mul_2 = tunum::mul(tunum::fe_holder{0.3}, -7);
    constexpr auto mul_3 = tunum::mul(2, 8.3f);
    constexpr auto mul_4 = tunum::mul(-1, 5ull);
    static_assert(std::is_same_v<typename decltype(mul_1)::calc_t, double>);
    static_assert(std::is_same_v<typename decltype(mul_2)::calc_t, double>);
    static_assert(std::is_same_v<typename decltype(mul_3)::calc_t, float>);
    static_assert(std::is_same_v<typename decltype(mul_4)::calc_t, float>);
    EXPECT_EQ(mul_1, 0.f * 3.3);
    EXPECT_EQ(mul_2, 0.3 * -7);
    EXPECT_EQ(mul_3, 2 * 8.3f);
    EXPECT_EQ(mul_4, -5.f);

    // 引数がNaNのみ、または有限数とNaN
    constexpr auto arg_nan1 = tunum::mul(float_info.get_nan(), 1.f);
    EXPECT_TRUE(tunum::floating_std_info{arg_nan1}.is_nan());
    EXPECT_FALSE(arg_nan1.has_fexcept());
    constexpr auto arg_nan2 = tunum::mul(1.f, float_info.get_nan(true));
    EXPECT_TRUE(tunum::floating_std_info{arg_nan2}.is_nan());
    EXPECT_FALSE(arg_nan2.has_fexcept());
    constexpr auto arg_nan3 = tunum::mul(float_info.get_nan(), float_info.get_nan(true));
    EXPECT_TRUE(tunum::floating_std_info{arg_nan3}.is_nan());
    EXPECT_FALSE(arg_nan3.has_fexcept());

    // 引数が0以上の有限数と無限大
    constexpr auto arg_inf1 = tunum::mul(float_info.get_infinity(), 1.f);
    EXPECT_TRUE(tunum::floating_std_info{arg_inf1}.is_infinity());
    EXPECT_TRUE(tunum::floating_std_info{arg_inf1}.sign() > 0);
    EXPECT_FALSE(arg_inf1.has_fexcept());
    constexpr auto arg_inf2 = tunum::mul(float_info.get_denormalized_min(true), float_info.get_infinity());
    EXPECT_TRUE(tunum::floating_std_info{arg_inf2}.is_infinity());
    EXPECT_TRUE(tunum::floating_std_info{arg_inf2}.sign() < 0);
    EXPECT_FALSE(arg_inf2.has_fexcept());

    // 引数が0と有限数
    constexpr auto arg_zero_zero = tunum::mul(0.f, 0.f);
    EXPECT_EQ(arg_zero_zero, 0);
    EXPECT_FALSE(arg_zero_zero.has_fexcept());
    constexpr auto arg_norm_zero = tunum::mul(float_info.get_max(), 0.f);
    EXPECT_EQ(arg_norm_zero, 0);
    EXPECT_FALSE(arg_norm_zero.has_fexcept());
    constexpr auto arg_zero_denorm = tunum::mul(0.f, -float_info.get_denormalized_min());
    EXPECT_EQ(arg_zero_denorm, 0);
    EXPECT_FALSE(arg_zero_denorm.has_fexcept());

    // 引数が0と無限大
    constexpr auto arg_zero_inf = tunum::mul(0.f, float_info.get_infinity());
    EXPECT_TRUE(tunum::floating_std_info{arg_zero_inf}.is_nan());
    EXPECT_FALSE(arg_zero_inf.has_inexact());
    EXPECT_FALSE(arg_zero_inf.has_divbyzero());
    EXPECT_TRUE(arg_zero_inf.has_invalid());
    EXPECT_FALSE(arg_zero_inf.has_overflow());
    EXPECT_FALSE(arg_zero_inf.has_underflow());

    // 無限大とNaN
    constexpr auto arg_nan_inf = tunum::mul(float_info.get_nan(), float_info.get_infinity());
    EXPECT_TRUE(tunum::floating_std_info{arg_nan_inf}.is_nan());
    EXPECT_FALSE(arg_nan_inf.has_fexcept());
    constexpr auto arg_inf_nan = tunum::mul(float_info.get_infinity(), float_info.get_nan());
    EXPECT_TRUE(tunum::floating_std_info{arg_inf_nan}.is_nan());
    EXPECT_FALSE(arg_inf_nan.has_fexcept());

    // 結果が非正規化数のアンダーフロー
    constexpr auto udf_1 = tunum::mul(0.5f, float_info.get_min());
    EXPECT_TRUE(tunum::floating_std_info{udf_1}.is_denormalized());
    EXPECT_TRUE(udf_1 > 0);
    EXPECT_TRUE(udf_1.has_inexact());
    EXPECT_FALSE(udf_1.has_divbyzero());
    EXPECT_FALSE(udf_1.has_invalid());
    EXPECT_FALSE(udf_1.has_overflow());
    EXPECT_TRUE(udf_1.has_underflow());
    // 結果が0のアンダーフロー
    constexpr auto udf_2 = tunum::mul(0.5f, float_info.get_denormalized_min());
    EXPECT_EQ(udf_2, 0);
    EXPECT_TRUE(udf_2.has_inexact());
    EXPECT_FALSE(udf_2.has_divbyzero());
    EXPECT_FALSE(udf_2.has_invalid());
    EXPECT_FALSE(udf_2.has_overflow());
    EXPECT_TRUE(udf_2.has_underflow());

    // オーバーフロー(正規化数最大値 * 浮動小数点型で表現可能な1より大きい隣接する値)
    constexpr auto ovf_1 = tunum::mul(float_info.get_max(), (float)tunum::floating_std_info<float>{false, 0, 1u});
    EXPECT_TRUE(tunum::floating_std_info{ovf_1}.is_infinity());
    EXPECT_TRUE(ovf_1.has_inexact());
    EXPECT_FALSE(ovf_1.has_divbyzero());
    EXPECT_FALSE(ovf_1.has_invalid());
    EXPECT_TRUE(ovf_1.has_overflow());
    EXPECT_FALSE(ovf_1.has_underflow());
    // 正規化数最大値 * 1 ではオーバーフローは発生しない
    constexpr auto non_ovf = tunum::mul(float_info.get_max(), 1.f);
    EXPECT_EQ(float_info.get_max(), non_ovf);
}

TEST(TunumFloatingTest, DivTest)
{
    constexpr auto float_info = tunum::floating_std_info{0.f};

    // 型推論と、結果が大きいほうの型に統一されているか
    // 整数同士の計算はfloatになるか
    constexpr auto div_1 = tunum::div(0.f, tunum::fe_holder{3.3});
    constexpr auto div_2 = tunum::div(tunum::fe_holder{0.3}, -7);
    constexpr auto div_3 = tunum::div(2, 8.3f);
    constexpr auto div_4 = tunum::div(-1, 5ull);
    static_assert(std::is_same_v<typename decltype(div_1)::calc_t, double>);
    static_assert(std::is_same_v<typename decltype(div_2)::calc_t, double>);
    static_assert(std::is_same_v<typename decltype(div_3)::calc_t, float>);
    static_assert(std::is_same_v<typename decltype(div_4)::calc_t, float>);
    EXPECT_EQ(div_1, 0.f / 3.3);
    EXPECT_EQ(div_2, 0.3 / -7);
    EXPECT_EQ(div_3, 2 / 8.3f);
    EXPECT_EQ(div_4, -1.f / 5.f);

    // 引数がNaNのみ、または有限数とNaN
    constexpr auto arg_nan1 = tunum::div(float_info.get_nan(), 1.f);
    EXPECT_TRUE(tunum::floating_std_info{arg_nan1}.is_nan());
    EXPECT_FALSE(arg_nan1.has_fexcept());
    constexpr auto arg_nan2 = tunum::div(1.f, float_info.get_nan(true));
    EXPECT_TRUE(tunum::floating_std_info{arg_nan2}.is_nan());
    EXPECT_FALSE(arg_nan2.has_fexcept());
    constexpr auto arg_nan3 = tunum::div(float_info.get_nan(), float_info.get_nan(true));
    EXPECT_TRUE(tunum::floating_std_info{arg_nan3}.is_nan());
    EXPECT_FALSE(arg_nan3.has_fexcept());

    // 0 / 0, 無限大 / 無限大
    constexpr auto arg_zero_zero = tunum::div(0.f, 0.f);
    EXPECT_TRUE(tunum::floating_std_info{arg_zero_zero}.is_nan());
    EXPECT_FALSE(arg_zero_zero.has_inexact());
    EXPECT_TRUE(arg_zero_zero.has_divbyzero());
    EXPECT_TRUE(arg_zero_zero.has_invalid());
    EXPECT_FALSE(arg_zero_zero.has_overflow());
    EXPECT_FALSE(arg_zero_zero.has_underflow());
    constexpr auto arg_inf_inf = tunum::div(float_info.get_infinity(), float_info.get_infinity());
    EXPECT_TRUE(tunum::floating_std_info{arg_inf_inf}.is_nan());
    EXPECT_FALSE(arg_inf_inf.has_inexact());
    EXPECT_FALSE(arg_inf_inf.has_divbyzero());
    EXPECT_TRUE(arg_inf_inf.has_invalid());
    EXPECT_FALSE(arg_inf_inf.has_overflow());
    EXPECT_FALSE(arg_inf_inf.has_underflow());

    // 無限大 / 有限数
    constexpr auto arg_inf_fin = tunum::div(float_info.get_infinity(), -float_info.get_denormalized_min());
    EXPECT_TRUE(tunum::floating_std_info{arg_inf_fin}.is_infinity());
    EXPECT_FALSE(arg_inf_fin.has_fexcept());
    // 有限数 / 0
    constexpr auto arg_fin_zero = tunum::div(float_info.get_max(), 0.f);
    EXPECT_TRUE(tunum::floating_std_info{arg_fin_zero}.is_infinity());
    EXPECT_FALSE(arg_fin_zero.has_inexact());
    EXPECT_TRUE(arg_fin_zero.has_divbyzero());
    EXPECT_FALSE(arg_fin_zero.has_invalid());
    EXPECT_FALSE(arg_fin_zero.has_overflow());
    EXPECT_FALSE(arg_fin_zero.has_underflow());

    // 無限大とNaN
    constexpr auto arg_nan_inf = tunum::div(float_info.get_nan(), float_info.get_infinity());
    EXPECT_TRUE(tunum::floating_std_info{arg_nan_inf}.is_nan());
    EXPECT_FALSE(arg_nan_inf.has_fexcept());
    constexpr auto arg_inf_nan = tunum::div(float_info.get_infinity(), float_info.get_nan());
    EXPECT_TRUE(tunum::floating_std_info{arg_inf_nan}.is_nan());
    EXPECT_FALSE(arg_inf_nan.has_fexcept());

    // ゼロとNaN
    constexpr auto arg_nan_zero = tunum::div(float_info.get_nan(), 0.f);
    EXPECT_TRUE(tunum::floating_std_info{arg_nan_zero}.is_nan());
    EXPECT_FALSE(arg_nan_zero.has_fexcept());
    constexpr auto arg_zero_nan = tunum::div(0.f, float_info.get_nan());
    EXPECT_TRUE(tunum::floating_std_info{arg_zero_nan}.is_nan());
    EXPECT_FALSE(arg_zero_nan.has_fexcept());

    // 結果が非正規化数のアンダーフロー
    constexpr auto udf_1 = tunum::div(float_info.get_min(), 2.f);
    EXPECT_TRUE(tunum::floating_std_info{udf_1}.is_denormalized());
    EXPECT_TRUE(udf_1 > 0);
    EXPECT_TRUE(udf_1.has_inexact());
    EXPECT_FALSE(udf_1.has_divbyzero());
    EXPECT_FALSE(udf_1.has_invalid());
    EXPECT_FALSE(udf_1.has_overflow());
    EXPECT_TRUE(udf_1.has_underflow());
    // 結果がゼロのアンダーフロー
    constexpr auto udf_2 = tunum::div(float_info.get_denormalized_min(), 2.f);
    EXPECT_EQ(udf_2, 0);
    EXPECT_TRUE(udf_2.has_inexact());
    EXPECT_FALSE(udf_2.has_divbyzero());
    EXPECT_FALSE(udf_2.has_invalid());
    EXPECT_FALSE(udf_2.has_overflow());
    EXPECT_TRUE(udf_2.has_underflow());

    // オーバーフロー(正規化数最大値 / 浮動小数点型で表現可能な1より小さい隣接する値)
    constexpr auto ovf_1 = tunum::div(float_info.get_max(), (float)float_info.make_mantissa_fullbit(false, -1));
    EXPECT_TRUE(tunum::floating_std_info{ovf_1}.is_infinity());
    EXPECT_TRUE(ovf_1.has_inexact());
    EXPECT_FALSE(ovf_1.has_divbyzero());
    EXPECT_FALSE(ovf_1.has_invalid());
    EXPECT_TRUE(ovf_1.has_overflow());
    EXPECT_FALSE(ovf_1.has_underflow());
    // 正規化数最大値 / 1 ではオーバーフローは発生しない
    constexpr auto non_ovf = tunum::div(float_info.get_max(), 1.f);
    EXPECT_EQ(float_info.get_max(), non_ovf);
}

TEST(TunumFloatingTest, FeHolderArithmeticOperatorTest)
{
    // 比較は上で行っている部分もあるので、何も起きなければ書かなくてよし
    // 単項+-

    // 下記算術演算子の細かいテストは専用のテスト関数で行う
    // ここでは、正常な呼び出しの確認が取れればOK

    // 加算

    // 減算

    // 乗算

    // 除算

}
