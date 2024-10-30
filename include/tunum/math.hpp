#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MATH_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MATH_HPP

#ifndef TUNUM_COMMON_INCLUDE
#define TUNUM_COMMON_INCLUDE(path) <tunum/path>
#endif

#include <stdexcept>

#include TUNUM_COMMON_INCLUDE(concepts.hpp)
#include TUNUM_COMMON_INCLUDE(math/exp.hpp)

namespace tunum
{
    // 絶対値取得
    inline constexpr auto abs(const TuArithmetic auto& v) { return (std::max)(v, -v); }

    // 自然対数の近似値算出
    // 参考: https://qiita.com/MilkySaitou/items/614fcbb110cae5b9f797
    // @tparam FloatT 任意の組み込み浮動小数点型
    // @param x 求めたい自然対数の真数
    // @param n 求める項の数(精度)。0の場合は、収束するまで計算を繰り返す
    template <std::floating_point FloatT>
    inline constexpr FloatT ln(FloatT x, std::size_t n = 0)
    {
        if (x <= 0)
            throw std::invalid_argument("'x' less than 0 cannot be specified.");

        const FloatT base_numerator = x - 1;
        if (abs(base_numerator) >= 1)
            return -ln<FloatT>(1 / x, n);
        // ln(1/10)
        if (x == .1)
            return -std::numbers::ln10_v<FloatT>;
        if (x < .1)
            return -std::numbers::ln10_v<FloatT> + ln<FloatT>(10 * x, n);
        // ln(1/5)
        if (x == .2)
            return -(std::numbers::ln10_v<FloatT> - std::numbers::ln2_v<FloatT>);
        if (x < .2)
            return ln<FloatT>(.2, n) + ln<FloatT>(5 * x, n);
        // ln(1/2)
        if (x == .5)
            return -std::numbers::ln2_v<FloatT>;
        if (x < .5)
            return -std::numbers::ln2_v<FloatT> + ln<FloatT>(2 * x, n);

        FloatT numerator = base_numerator;
        FloatT total = 0;
        for (int i = 1, sign = 1; n == 0 || i < n; i++) {
            const FloatT before_total = total;
            total += (sign * numerator / i);

            // 計算結果に変化がなくなった場合、収束したとして処理を終了
            if (before_total == total)
                break;

            sign *= -1;
            numerator *= base_numerator;
        }
        return total;
    }
    inline constexpr auto ln(std::integral auto x, std::size_t n = 0) { return ln(static_cast<double>(x), n); }

    // 対数
    // @param base 対数の底
    // @param x 求めたい対数の真数
    // @param n 求める項の数(精度)
    inline constexpr auto log(auto base, auto x, std::size_t n = 0) { return ln(x, n) / ln(base, n); }

    // 常用対数
    // @param x 求めたい自然対数の真数
    // @param n 求める項の数(精度)
    inline constexpr auto log10(auto x, std::size_t n = 0) { return log(10, x, n); }

    // 小数点以下切り捨て
    inline constexpr auto floor(std::floating_point auto v) { return static_cast<std::int64_t>(v); }

    // 小数点以下切り上げ
    inline constexpr auto ceil(std::floating_point auto v)
    {
        const auto floored = tunum::floor(v);
        return floored < v
            ? floored + 1
            : floored;
    }

    namespace numbers
    {
        // ---------------------------
        // 数値の定数定義
        // ---------------------------

        inline constexpr double log10_2 = std::numbers::ln2 / std::numbers::ln10;
    }
}

#endif
