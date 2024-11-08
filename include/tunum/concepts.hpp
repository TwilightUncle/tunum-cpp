// ---------------------------------------------
// 制約
// ---------------------------------------------
#ifndef TUNUM_INCLUDE_GUARD_TUNUM_CONCEPTS_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_CONCEPTS_HPP

#ifndef TUNUM_COMMON_INCLUDE
#define TUNUM_COMMON_INCLUDE(path) <tunum/path>
#endif

#include <iterator>
#include TUNUM_COMMON_INCLUDE(mp.hpp)

namespace tunum
{
    // fmpint かどうか判定
    template <class T>
    concept TuFmpIntegral = is_fmpint_v<T>;

    // fmpint または 組み込み整数型 かどうか判定
    template <class T>
    concept TuIntegral = is_fmpint_v<T> || std::is_integral_v<T>;

    // 符号なし fmpint か判定
    template <class T>
    concept TuFmpUnsigned = is_unsigned_fmpint_v<T>;

    // 符号なしの fmpint または、符号なし整数か判定
    template <class T>
    concept TuUnsigned = is_unsigned_fmpint_v<T> || std::is_unsigned_v<T>;

    // bit演算可能なことの制約
    template <class T>
    concept TuBitwiseOperable = requires (T& v) {
        { v >> std::declval<int>() } -> std::convertible_to<T>;
        { v << std::declval<int>() } -> std::convertible_to<T>;
        { ~v } -> std::convertible_to<T>;
        { v & std::declval<T>() } -> std::convertible_to<T>;
        { v | std::declval<T>() } -> std::convertible_to<T>;
        { v ^ std::declval<T>() } -> std::convertible_to<T>;
    };

    // 算術型か、整数値と下記の操作ができる型
    // - 全順序比較
    // - 四則演算
    // - 単項 マイナス
    template <class T>
    concept TuArithmetic
        = std::is_arithmetic_v<T>
        || requires (T v) {
            { -v } -> std::convertible_to<T>;
            { v += std::declval<T>() } -> std::same_as<T&>;
            { v -= std::declval<T>() } -> std::same_as<T&>;
            { v *= std::declval<T>() } -> std::same_as<T&>;
            { v /= std::declval<T>() } -> std::same_as<T&>;
            { v + std::declval<T>() } -> std::convertible_to<T>;
            { v - std::declval<T>() } -> std::convertible_to<T>;
            { v * std::declval<T>() } -> std::convertible_to<T>;
            { v / std::declval<T>() } -> std::convertible_to<T>;
        }
        && std::totally_ordered<T>
        && std::totally_ordered_with<T, int>
        && std::convertible_to<T, int>
        && std::convertible_to<int, T>;

    // 整数の配列やコンテナ(添え字アクセスが可能なこと)
    template <class T>
    concept TuAnyBaseNumbers = requires (T& v) {
        { std::size(v) } -> std::convertible_to<std::size_t>;
        { v[std::declval<std::size_t>()] } -> std::convertible_to<int>;
        { *std::begin(v) } -> std::convertible_to<int>;
        std::end(v);
    };

    // 算術的な関数
    template <class F, class R, class... Args>
    concept TuArithmeticInvocable
        = TuArithmetic<R>
        && (... && TuArithmetic<Args>)
        && (... && std::convertible_to<Args, R>)
        && std::regular_invocable<F, Args...>
        && std::is_invocable_r_v<R, F, Args...>;

    // 戻り値の検証を含めた関数
    template <class F, class R, class... Args>
    concept TuInvocableR = std::is_invocable_r_v<R, F, Args...>;
}

#endif
