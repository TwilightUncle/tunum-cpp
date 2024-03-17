#ifndef TUNUM_INCLUDE_GUARD_TUNUM_UINT_BASE_NUMBER_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_UINT_BASE_NUMBER_HPP

#ifndef TUNUM_COMMON_INCLUDE
#define TUNUM_COMMON_INCLUDE(path) <tunum/path>
#endif

#include <array>

#include TUNUM_COMMON_INCLUDE(fmpint.hpp)

// 符号なし整数に対しての進数操作
namespace tunum::uibn
{
    // 進数変換後の桁数を計算
    // @param from_base 変換前の進数
    // @param to_base 変換後の進数
    // @param from_numbers 変換前の数値列
    inline constexpr std::size_t calc_changed_base_number_digits(
        std::size_t from_base,
        std::size_t to_base,
        std::size_t from_digits
    ) {
        if (from_base == to_base)
            return from_digits;

        const double rate = ::tunum::ln<double>(from_base) / ::tunum::ln<double>(to_base);
        const double logical_digits_size = static_cast<double>(from_digits) * rate;
        return ::tunum::ceil(logical_digits_size);
    }

    // 数値配列の内容を検証  
    // TODO: 扱うことのできる進数について上限も設けたほうが良いか？
    // @tparam Base 入力した数値列の進数
    template <std::size_t Base, std::unsigned_integral T, std::size_t N>
    requires (Base > 1)
    inline constexpr bool validate_numbers(const std::array<T, N>& numbers)
    {
        // 格納されている値が該当進数の一桁に収まっているか検証
        for (T num : numbers)
            if (num >= Base)
                return false;
        return true;
    }

    // 数値列を整数型へ格納
    template <std::size_t Base, std::unsigned_integral T, std::size_t N>
    requires (Base > 1)
    inline constexpr auto convert_numbers_to_integer(const std::array<T, N>& numbers)
    {
        if (!validate_numbers<Base>(numbers))
            throw std::invalid_argument("Specified invalid numbers.");

        // 最大ビット幅
        constexpr auto bit_length = calc_changed_base_number_digits(Base, 2, N);
        using numbers_storable_uint_t = fmpint<::tunum::calc_integral_storable_byte<2>(bit_length), false>;
        using uint_t = std::conditional_t<
            (bit_length < 64),
            std::uint64_t,
            numbers_storable_uint_t
        >;

        uint_t r{};
        for (uint_t coe = 1; auto num : numbers) {
            r += coe * num;
            coe *= Base;
        }
        return r;
    }

    // 整数型を任意進数の数値列へ変換
    // 整数型は各bitを1桁とした2進数数値列として扱う
    template <std::size_t ToBase, std::unsigned_integral T = unsigned int>
    requires (ToBase > 1)
    inline constexpr auto change_base_number(::tunum::TuUnsigned auto v)
    {
        using uint_t = decltype(v);
        constexpr auto bitwidth = []() -> std::size_t {
            if constexpr (std::integral<uint_t>)
                return sizeof(uint_t);
            else
                return uint_t::max_digits2;
        }();

        // 変換後の桁数
        constexpr auto changed_digits = calc_changed_base_number_digits(2, ToBase, bitwidth);
        using result_t = std::array<T, changed_digits>;
        result_t r = {};
        for (auto& num : r) {
            num = v % ToBase;
            v /= ToBase;
        }
        return r;
    }

    // 進数変換実施
    // @tparam FromBase 変換前の進数
    // @tparam ToBase 変換後の進数
    // @tparam T 数値配列の要素の型
    // @tparam N 入力する数値配列のサイズ
    template <std::size_t FromBase, std::size_t ToBase, std::unsigned_integral T, std::size_t N>
    requires (FromBase > 1 && ToBase > 1)
    inline constexpr auto change_base_number(const std::array<T, N>& numbers)
    {
        if constexpr (FromBase == ToBase)
            return std::array<T, N>{numbers};
        else
            return change_base_number<ToBase, T>(
                convert_numbers_to_integer<FromBase>(numbers)
            );
    }
}

#endif
