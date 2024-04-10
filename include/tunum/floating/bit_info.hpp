#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_BIT_INFO_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_BIT_INFO_HPP

#include TUNUM_COMMON_INCLUDE(bit.hpp)

namespace tunum
{
    // 浮動小数点型の内部表現解析クラス
    // 基数が2であることを前提として解釈する
    template <std::size_t ValueBitWidth, std::size_t MantissaBitWidth, TuIntegral auto MaxExponent>
    struct floating_bit_info
    {
        static constexpr auto bit_width = ValueBitWidth;
        static constexpr auto mantissa_width = MantissaBitWidth;
        static constexpr auto max_exponent = MaxExponent;
        static constexpr auto max_bit_width = std::bit_ceil(bit_width);
        static constexpr auto data_storeble_bytes = max_bit_width >> 3;

        using data_store_t = get_uint_t<data_storeble_bytes>;
        using exponent_value_t = get_int_t<sizeof(MaxExponent)>;

        // 内部表現抽出用パラメータの制約
        // * 仮数部のビット幅が値のビット幅より小さく、符号部も存在できるようなサイズであること
        static_assert(bit_width > 0);
        static_assert(mantissa_width > 0 && mantissa_width < bit_width - 1);
        static_assert(max_exponent >= 0);

        // 内部表現抽出用のbitマスク定義
        static constexpr data_store_t data_mask = max_bit_width == bit_width
            ? ~data_store_t{}
            : ~(~data_store_t{} << (bit_width % max_bit_width));
        static constexpr data_store_t sign_mask = data_store_t{1} << (bit_width - 1);
        static constexpr data_store_t mantissa_mask = ~(~data_store_t{} << mantissa_width);
        static constexpr data_store_t exponent_mask = data_mask ^ (sign_mask | mantissa_mask);

        // メンバ変数
        data_store_t data = {};

        // --------------------------------
        // コンストラクタ
        // --------------------------------

        constexpr floating_bit_info() = default;
        constexpr floating_bit_info(data_store_t v) noexcept
            : data(v)
        {}

        // --------------------------------
        // ビット表現の抽出
        // --------------------------------

        // 指数部のビット表現
        constexpr data_store_t exponent_bits() const noexcept
        { return (data & exponent_mask) >> mantissa_width; }

        // 仮数部のビット表現
        constexpr data_store_t mantissa_bits() const noexcept
        { return data & mantissa_mask; }

        // --------------------------------
        // bit表現をもとにデータの種類を判定
        // --------------------------------

        // 指数部の全てのビットが立っている
        constexpr bool is_exponent_full() const noexcept
        { return (data & exponent_mask) == exponent_mask; }

        // ゼロかどうか
        constexpr bool is_zero() const noexcept
        { return !exponent_bits() && !mantissa_bits(); }

        // 非正規化数かどうか
        constexpr bool is_unnormalized() const noexcept
        { return !exponent_bits() && static_cast<bool>(mantissa_bits()); }

        // 正規化数かどうか
        constexpr bool is_normalized() const noexcept
        { return !exponent_bits() == is_exponent_full(); }

        // 無限大かどうか
        constexpr bool is_infinity() const noexcept
        { return is_exponent_full() && !mantissa_bits(); }

        // NaNかどうか
        constexpr auto is_nan() const noexcept
        { return is_exponent_full() && static_cast<bool>(mantissa_bits()); }

        // --------------------------------
        // 具体的な値を取得
        // --------------------------------

        // 符号取得(1 or -1)
        constexpr int sign() const noexcept
        {
            return static_cast<bool>(data & sign_mask)
                ? -1
                : 1;
        }

        // 指数
        constexpr exponent_value_t exponent() const noexcept
        {
            constexpr auto bias = max_exponent + mantissa_width;
            if (is_unnormalized())
                return exponent_value_t{1} - exponent_value_t{bias};
            if (is_normalized())
                return exponent_value_t{exponent_bits()} - exponent_value_t{bias};
            if (is_zero())
                return 0;
            return 1;
        }

        // 仮数
        constexpr data_store_t mantissa() const noexcept
        {
            if (is_normalized())
                return mantissa_bits() | (data_store_t{1} << mantissa_width);
            return mantissa_bits();
        }
    };
}

#endif
