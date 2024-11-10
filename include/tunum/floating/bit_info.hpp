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
        // コンストラクタ、演算子等
        // --------------------------------

        constexpr floating_bit_info() = default;
        constexpr floating_bit_info(data_store_t v) noexcept
            : data(v)
        {}
        // 解析対象型による直接のオブジェクト構築
        constexpr floating_bit_info(T v) noexcept
            : data(std::bit_cast<data_store_t, T>(v))
        {}
        constexpr floating_bit_info(bool signbit, exponent_value_t exp, data_store_t mantissa) noexcept
            : data(make_direct_bits(signbit, exp, mantissa))
        {}

        constexpr explicit operator T() const noexcept
        { return std::bit_cast<T, data_store_t>(this->data); }

        // --------------------------------
        // ビット表現の抽出
        // --------------------------------

        // 符号ビット、指数、仮数部のビット列を直接指定してデータ作成
        static constexpr data_store_t make_direct_bits(bool signbit, exponent_value_t exp, data_store_t mantissa) noexcept
        {
            // 指数の最大値超過の場合無限大
            if (exp > max_exponent)
                return infinity_bits(signbit);

            const auto sign_part = signbit * sign_mask;
            // ここが負の値になると、data_store_tキャスト時に値がおかしくなるため、maxで補正
            const auto nonbias_exponent = (std::max)(0, exp + bias());
            const auto exponent_part = static_cast<data_store_t>(nonbias_exponent) << mantissa_width;
            return sign_part
                | (exponent_part & exponent_mask)
                | (mantissa & mantissa_mask); 
        }

        static constexpr data_store_t infinity_bits(bool signbit) noexcept
        { return (sign_mask * signbit) | exponent_mask; }

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
        static constexpr auto bias(bool is_floating_mantissa = true) noexcept
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

        // 小数部が存在するかどうか
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

        // --------------------------------
        // 内部表現ををデータがわかるように取得
        // --------------------------------

        // 符号部をマスクしたデータをそのまま抽出
        constexpr data_store_t sign_raw() const noexcept
        { return data & sign_mask; }

        // 符号取得(1 or -1)
        constexpr int sign() const noexcept
        {
            return static_cast<bool>(sign_raw())
                ? -1
                : 1;
        }

        // 指数
        // 引数は仮数部を小数点表記した場合の桁数で見るかどうか
        constexpr exponent_value_t exponent(bool is_floating_mantissa = true) const noexcept
        {
            return is_finity()
                ? exponent_value_t(exponent_bits()) - bias(is_floating_mantissa)
                : 1;
        }

        // 仮数
        constexpr data_store_t mantissa() const noexcept
        {
            return is_normalized()
                ? mantissa_bits() | (data_store_t{1} << mantissa_width)
                : mantissa_bits();
        }

        // --------------------------------
        // 生成、変更
        // --------------------------------

        // 同じデータで新規オブジェクト生成
        constexpr floating_bit_info clone() const noexcept
        { return {data}; }

        // 符号部のみ変更
        constexpr floating_bit_info change_sign(bool signbit) const noexcept
        { return {(signbit * sign_mask) | (data & ~sign_mask)}; }

        // 指数部のみ変更
        constexpr floating_bit_info change_exponent(exponent_value_t exp) const noexcept
        {
            return exp != exponent()
                ? floating_bit_info{sign() < 0, exp, mantissa_bits()}
                : clone();
        }

        // 仮数部のみ変更
        constexpr floating_bit_info change_mantissa(data_store_t mantissa) const noexcept
        { return {sign() < 0, exponent(), mantissa}; }

        // 指数部に加算
        constexpr floating_bit_info add_exponent(exponent_value_t x) const noexcept
        { return change_exponent(exponent() + x); }

        // 無限大の解釈オブジェクト生成
        static constexpr floating_bit_info make_infinity(bool signbit = false) noexcept
        { return {infinity_bits(signbit)}; }

        // Not a numberの解釈オブジェクト生成
        static constexpr floating_bit_info make_nan(bool signbit = false) noexcept
        { return make_infinity(signbit).change_mantissa(1); }

        // ゼロの解釈オブジェクト生成
        static constexpr floating_bit_info make_zero(bool signbit = false) noexcept
        { return floating_bit_info{}.change_sign(signbit); }

        // 非正規化数の最小値の解釈オブジェクト生成
        static constexpr floating_bit_info make_denormalized_min(bool signbit = false) noexcept
        { return floating_bit_info{data_store_t{1}}.change_sign(signbit); }

        // 非正規化数の最大値の解釈オブジェクト生成
        static constexpr floating_bit_info make_denormalized_max(bool signbit = false) noexcept
        { return {~make_infinity(!signbit).data}; }

        // 正規化数最小値の解釈オブジェクト生成
        static constexpr floating_bit_info make_min(bool signbit = false) noexcept
        { return {make_denormalized_max(signbit).data + 1}; }

        // 正規化数最大値の解釈オブジェクト生成
        static constexpr floating_bit_info make_max(bool signbit = false) noexcept
        { return {~make_min(!signbit).data}; }

        // 整数部分のみのオブジェクト作成
        constexpr floating_bit_info make_integral_part() const noexcept
        {
            // 0を除く有限数以外はそのまま返却
            if (!is_finity() || is_zero())
                return clone();
            // 指数が0以下の場合または非正規化数は整数が存在しないので、符号部のみ残して、返却
            return is_denormalized() || exponent() < 0
                ? make_zero(sign() < 0)
                : change_mantissa(mantissa_integral_bits());
        }

        // 小数部分のみのオブジェクト作成
        constexpr floating_bit_info make_decimal_part() const noexcept
        {
            // 無限大は符号部のみ残して、0を返却
            if (is_infinity())
                return make_zero(sign() < 0);
            // 正規化数以外は整数部が存在しないか、nanなのでそのまま値を返却
            return !is_normalized() || exponent() < 0
                ? clone()
                : floating_bit_info{T(*this) - T(make_integral_part())};
        }

        // 最も近い表現可能な値のbit表現を取得
        // なお、非正規化数サポート有無による動作の違いがどうなっているかよくわからん
        // @param direction 0の場合そのままの値、正の場合、大きいほうの隣接値、負の場合小さいほうの隣接値
        constexpr floating_bit_info next(float direction) const noexcept
        {
            if (!is_finity() || direction == 0)
                return clone();

            // 0をマイナス方向に移動させる場合または、dataの符号が負の場合は、方向を反転して計算
            // ※整数と違い、画一的に計算できない
            if (sign() < 0 || is_zero() && direction < 0)
                return change_sign(false)
                    .next(-direction)
                    .change_sign(true);

            const auto delta = direction > 0 ? 1 : -1;
            const auto next_mantissa = mantissa() + delta;
            const auto next_mantissa_width = std::bit_width(next_mantissa);
            const auto is_change_mantissa_width = next_mantissa_width != (mantissa_width + 1);
            // 正規化数はケチ表現で最大ビットが常に付与されるため、mantissa_width + 1からビット幅が変化したとき、指数も変わる
            // 一方で非正規化数は、最大ビットの付与がなく、正規化数との境界のみで指数が変わる
            // zeroの場合は必ずfalse, infおよびnanで個々の処理が走ってはならない
            const bool is_change_exponent = is_normalized() == is_change_mantissa_width;
            return change_mantissa(next_mantissa)
                .add_exponent(delta * is_change_exponent);
        }
    };
}

#endif
