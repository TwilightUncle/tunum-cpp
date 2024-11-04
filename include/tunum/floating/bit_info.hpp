#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_BIT_INFO_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_BIT_INFO_HPP

#include <cmath>
#include TUNUM_COMMON_INCLUDE(bit.hpp)

namespace tunum
{
    template <class T>
    concept FloatingInfomation = requires (T& v) {
        typename T::data_store_t;
        typename T::exponent_value_t;
        // { std::declval<typename T::data_store_t>() } -> TuUnsigned;
        // { std::declval<typename T::exponent_value_t>() } -> TuIntegral;
        { v.is_zero() } -> std::convertible_to<bool>;
        { v.is_denormalized() } -> std::convertible_to<bool>;
        { v.is_normalized() } -> std::convertible_to<bool>;
        { v.is_infinity() } -> std::convertible_to<bool>;
        { v.is_nan() } -> std::convertible_to<bool>;
        { v.get_fpclass() } -> std::convertible_to<int>;
        { v.sign() } -> std::convertible_to<int>;
        { v.exponent() } -> std::convertible_to<typename T::exponent_value_t>;
        { v.mantissa() } -> std::convertible_to<typename T::data_store_t>;
        { v.has_decimal_part() } -> std::convertible_to<bool>;
    };

    // 浮動小数点型の内部表現解析クラス
    // 基数が2であることを前提として解釈する
    template <class T, class LimitsT = std::numeric_limits<T>>
    struct floating_bit_info
    {
        static constexpr auto bit_width = sizeof(T) << 3;
        // numeric_limitではケチ表現の分、1桁多いためマイナス1
        static constexpr auto mantissa_width = LimitsT::digits - 1;
        static constexpr auto max_exponent = LimitsT::max_exponent - 1;
        static constexpr auto max_bit_width = std::bit_ceil(bit_width);
        static constexpr auto data_storeble_bytes = max_bit_width >> 3;

        using data_store_t = get_uint_t<data_storeble_bytes>;
        using exponent_value_t = get_int_t<sizeof(max_exponent)>;

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
        // 各種表現ごとに直接指定
        constexpr floating_bit_info(bool s, exponent_value_t e, data_store_t m, bool is_floating_mantissa = true) noexcept
        {
            const auto sign_part = s ? sign_mask : data_store_t{};
            const auto exponent_part = data_store_t{e + bias(is_floating_mantissa)} << mantissa_width;
            data = sign_part
                | (exponent_part & exponent_mask)
                | (m & mantissa_mask);
        }

        // --------------------------------
        // ビット表現の抽出
        // --------------------------------

        // 指数部のビット表現
        constexpr data_store_t exponent_bits() const noexcept
        { return (data & exponent_mask) >> mantissa_width; }

        // 仮数部のビット表現
        constexpr data_store_t mantissa_bits() const noexcept
        { return data & mantissa_mask; }

        // 仮数部の小数部分bit(正規化数の時のみ有効)
        constexpr data_store_t mantissa_decimal_bits() const noexcept
        {
            // 抽出ビットマスク生成用のm議シフト数取得
            const auto shiftr_n = (std::max)(
                0,
                (std::min)(exponent(), exponent_value_t(mantissa_width))
            );
            // 右シフトはあまり使いたくないので、シフト数を左シフトに変換して、マスク生成
            const auto mask = ~(~data_store_t{} << (mantissa_width - shiftr_n));
            return mantissa_bits() & mask;
        }

        // 仮数部の整数部分bit(正規化数の時のみ有効)
        constexpr data_store_t mantissa_integral_bits() const noexcept
        { return mantissa_bits() & ~mantissa_decimal_bits(); }

        // --------------------------------
        // 実装時の補助関数
        // --------------------------------

        // 指数部の全てのビットが立っている
        constexpr bool is_exponent_full() const noexcept
        { return (data & exponent_mask) == exponent_mask; }

        // 指数にかかるバイアスを取得
        constexpr auto bias(bool is_floating_mantissa = true) const noexcept
        {
            const auto b = is_floating_mantissa
                ? max_exponent
                : max_exponent + mantissa_width;
            return exponent_value_t(b);
        }

        // --------------------------------
        // bit表現をもとにデータの種類を判定
        // --------------------------------

        // ゼロかどうか
        constexpr bool is_zero() const noexcept
        { return !exponent_bits() && !mantissa_bits(); }

        // 非正規化数かどうか
        constexpr bool is_denormalized() const noexcept
        { return !exponent_bits() && static_cast<bool>(mantissa_bits()); }

        // 正規化数かどうか
        constexpr bool is_normalized() const noexcept
        { return !exponent_bits() == is_exponent_full(); }

        // 有限かどうか
        constexpr bool is_finity() const noexcept
        { return is_zero() || is_denormalized() || is_normalized(); }

        // 無限大かどうか
        constexpr bool is_infinity() const noexcept
        { return is_exponent_full() && !mantissa_bits(); }

        // NaNかどうか
        constexpr auto is_nan() const noexcept
        { return is_exponent_full() && static_cast<bool>(mantissa_bits()); }

        // 保持している値の種類を示す列挙体返却
        constexpr auto get_fpclass() const noexcept
        {
            if (is_zero())
                return FP_ZERO;
            if (is_normalized())
                return FP_NORMAL;
            if (is_denormalized())
                return FP_SUBNORMAL;
            return is_infinity()
                ? FP_INFINITE
                : FP_NAN;
        }

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
        // 引数は仮数部を小数点表記した場合の桁数で見るかどうか
        constexpr exponent_value_t exponent(bool is_floating_mantissa = true) const noexcept
        {
            if (is_denormalized())
                return exponent_value_t(1) - bias(is_floating_mantissa);
            if (is_normalized())
                return exponent_value_t(exponent_bits()) - bias(is_floating_mantissa);
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

        // 少数部が存在するかどうか
        constexpr bool has_decimal_part() const noexcept
        {
            // 正規化数以外は自明(あるいは未定義)として、
            // その場で結果返却
            // ※非正規化数は非常に小さい値のため、小数点以下を保持しているとする
            if (!is_normalized())
                return is_denormalized();

            const auto using_mantissa_digits = (std::max)(
                int(mantissa_width) - countr_zero(mantissa_bits()),
                0
            );
            // 使用中のビット幅よりも指数(左シフト数)が小さいとき、
            // 少数部が存在するものと判定
            return exponent() < using_mantissa_digits;
        }

        // 整数部分のbit列抽出
        constexpr data_store_t get_integral_part_bits() const noexcept
        {
            // 0を除く有限数以外はそのまま返却
            if (!is_finity() || is_zero())
                return data;
            // 指数が0以下の場合または非正規化数は整数が存在しないので、符号部のみ残して、返却
            return is_denormalized() || exponent() < 0
                ? data & sign_mask
                : (data & ~mantissa_mask) | mantissa_integral_bits();
        }

        // 最も近い表現可能な値のbit表現を取得
        // なお、非正規化数サポート有無による動作の違いがどうなっているかよくわからん
        // @param is_direction_plus 真の場合、大きいほうの隣接値、偽の場合小さいほうの隣接値
        constexpr data_store_t next_bits(bool is_direction_plus) const noexcept
        {
            // 0をマイナス方向に移動させる場合または、dataの符号が負の場合は、方向を反転して計算
            // ※整数と違い、画一的に計算できない
            if (sign() < 0 || is_zero() && !is_direction_plus)
                return sign_mask | floating_bit_info{data & (~sign_mask)}.next_bits(!is_direction_plus);

            const auto delta = is_direction_plus ? 1 : -1;
            const auto next_mantissa = mantissa() + delta;
            // 正規化数はケチ表現で最大ビットが常に付与されるため、mantissa_width + 1からビット幅が変化したとき、指数も変わる
            // 一方で非正規化数は、最大ビットの付与がなく、正規化数との境界のみで指数が変わる
            const bool is_change_exponent = is_normalized() && std::bit_width(next_mantissa) != (mantissa_width + 1)
                || is_denormalized() && std::bit_width(next_mantissa) == (mantissa_width + 1);

            if (is_change_exponent) {
                const auto result_exp = (exponent_bits() + delta) << mantissa_width;
                return (data & sign_mask) | (result_exp & exponent_mask) | (next_mantissa & mantissa_mask);
            }
            return (data & ~mantissa_mask) | (next_mantissa & mantissa_mask);
        }
    };
}

#endif
