#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_CORE_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_CORE_HPP

#include TUNUM_COMMON_INCLUDE(floating.hpp)
#include TUNUM_COMMON_INCLUDE(number_array.hpp)
#include TUNUM_COMMON_INCLUDE(fmpint/impl/arithmetic.hpp)

#include <array>
#include <stdexcept>
#include <compare>
#include <limits>

namespace tunum
{
    // -------------------------------------------
    // クラス実装
    // -------------------------------------------

    // 固定サイズの多倍長整数
    // 内部的な演算方法は組み込みの整数に準拠
    // TODO: 組み込み浮動小数点型とのキャスト関連を考える
    // TODO: 文字列からの構築時、Ryuあたりのアルゴリズムを用いて効率化できるか調べる
    template <std::size_t Bytes, bool Signed = false>
    struct fmpint
    {
        // -------------------------------------------
        // メンバ定義
        // -------------------------------------------

        // 内部的に扱うサイズは2の累乗に統一する
        using base_data_t = std::uint32_t;
        static constexpr std::size_t min_size = sizeof(base_data_t) << 1;
        static constexpr std::size_t size = (std::max)(std::bit_ceil(Bytes), min_size);
        static constexpr std::size_t half_size = size >> 1;
        static constexpr std::size_t data_length = size / sizeof(base_data_t);

        static constexpr std::size_t base_data_digits2 = std::numeric_limits<base_data_t>::digits;
        static constexpr std::size_t max_digits2 = size * 8;
        static constexpr std::size_t max_digits10 = calc_integral_digits_from_bitwidth<10>(max_digits2);

        using min_size_fmpint = fmpint<(sizeof(base_data_t) << 1), false>;
        using half_fmpint = std::conditional_t<
            half_size == sizeof(base_data_t),
            base_data_t,
            fmpint<half_size, false>
        >;

        static constexpr bool is_min_size = std::same_as<base_data_t, half_fmpint>;

        half_fmpint lower = {};
        half_fmpint upper = {};

        // -------------------------------------------
        // コンストラクタ
        // -------------------------------------------

        constexpr fmpint() = default;

        // 組み込みの整数から生成
        constexpr fmpint(std::integral auto v) noexcept
            : lower(v)
            , upper((v < 0) ? ~half_fmpint{} : half_fmpint{})
        {
            // このクラスも、指定された整数どちらも64ビットの場合のみ
            if constexpr (size == sizeof(v))
                this->upper = std::rotl(v, half_size * 8);
        }

        // 異なる符号同士は引数の符号を反転したうえで別コンストラクタに委譲
        template <std::size_t N>
        constexpr fmpint(const fmpint<N, !Signed>& v) noexcept
            : fmpint(v._switch_sign())
        {}

        // 異なるサイズのfmpintから生成(内部表現が等しい)
        template <std::size_t N>
        requires (Bytes != N && fmpint<N, Signed>::size == size)
        constexpr fmpint(const fmpint<N, Signed>& v) noexcept
            : lower(v.lower)
            , upper(v.upper)
        {}

        // 異なるサイズのfmpintから生成(内部表現が小さい)
        template <std::size_t N>
        requires (fmpint<N, Signed>::size < size)
        constexpr fmpint(const fmpint<N, Signed>& v) noexcept
            : lower(v._to_unsigned())
            , upper(v._is_minus() ? ~half_fmpint{} : half_fmpint{})
        {}

        // 異なるサイズのfmpintから生成
        // 格納できない領域は破棄
        template <std::size_t N>
        requires (fmpint<N, Signed>::size > size)
        constexpr fmpint(const fmpint<N, Signed>& v) noexcept
            : fmpint(v.lower)
        {}

        // 2つの整数より、upperとlowerを直接セット
        constexpr fmpint(std::integral auto u, std::integral auto l)
            : lower(l)
            , upper(u)
        {}

        // 2つの整数より、upperとlowerを直接セット
        template <std::size_t N1, std::size_t N2>
        constexpr fmpint(const fmpint<N1, Signed>& u, const fmpint<N2, Signed>& l)
            : lower(l)
            , upper(u)
        {}

        // c言語風文字列より初期化
        template <class CharT>
        constexpr fmpint(const CharT* num_str)
            : fmpint(std::basic_string_view<CharT>{num_str})
        {}

        // basic_string_viewあたりを引数として、文字列からの実装を想定
        template <class CharT, class Traits = std::char_traits<CharT>>
        constexpr fmpint(std::basic_string_view<CharT, Traits> num_str)
        {
            // 数値リテラルの接頭詞
            constexpr auto digits2_prefix_1 = TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, Traits, "0b");
            constexpr auto digits2_prefix_2 = TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, Traits, "0B");
            constexpr auto digits8_prefix_1 = TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, Traits, "0");
            constexpr auto digits16_prefix_1 = TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, Traits, "0x");
            constexpr auto digits16_prefix_2 = TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, Traits, "0X");

            if (num_str.starts_with(digits2_prefix_1) || num_str.starts_with(digits2_prefix_2)) {
                constexpr auto arr_size = calc_integral_digits_from_bitwidth<2>(max_digits2);
                (*this) = _make_by_digits_power2_arr<2>(convert_str_to_number_array<arr_size>(num_str, 2));
            }
            else if (num_str.starts_with(digits16_prefix_1) || num_str.starts_with(digits16_prefix_2)) {
                constexpr auto arr_size = calc_integral_digits_from_bitwidth<16>(max_digits2);
                (*this) = _make_by_digits_power2_arr<16>(convert_str_to_number_array<arr_size>(num_str, 16));
            }
            else if (num_str.starts_with(digits8_prefix_1)) {
                constexpr auto arr_size = calc_integral_digits_from_bitwidth<8>(max_digits2);
                (*this) = _make_by_digits_power2_arr<8>(convert_str_to_number_array<arr_size>(num_str, 8));
            }
            else {
                constexpr auto arr_size = calc_integral_digits_from_bitwidth<10>(max_digits2);
                (*this) = _make_by_digits10_arr(convert_str_to_number_array<arr_size>(num_str, 10));
            }
        }

        // 組み込みの浮動小数点型から生成
        constexpr fmpint(std::floating_point auto v) noexcept
        {
            const auto info = floating_std_info{v};
            (*this) = _make_by_floating_info(info, v);
        }

        // -------------------------------------------
        // 要素アクセス
        // -------------------------------------------

        // 内部表現へのアクセス
        constexpr const base_data_t& at(std::size_t i) const
        {
            if (i >= data_length) throw std::out_of_range{""};
            return (*this)[i];
        }
        constexpr base_data_t& at(std::size_t i)
        {
            if (i >= data_length) throw std::out_of_range{""};
            return (*this)[i];
        }

        // 内部表現のうち一番小さい値の要素にアクセス
        constexpr const base_data_t& front() const noexcept
        { return this->at(0); }
        constexpr base_data_t& front() noexcept
        { return this->at(0); }

        // 内部表現のうち一番大きい値の要素にアクセス
        constexpr const base_data_t& back() const noexcept
        { return this->at(data_length - 1); }
        constexpr base_data_t& back() noexcept
        { return this->at(data_length - 1); }

        // -------------------------------------------
        // 演算子オーバーロード
        // -------------------------------------------

        constexpr const base_data_t& operator[](std::size_t n) const
        { return _at(this, n); }
        constexpr base_data_t& operator[](std::size_t n)
        { return _at(this, n); }

        // 組み込み整数へのキャスト
        constexpr explicit operator std::uint64_t() const noexcept
        {
            return (std::uint64_t{(*this)[1]} << base_data_digits2)
                | std::uint64_t{(*this)[0]};
        }
        constexpr explicit operator std::uint32_t() const noexcept
        { return this->front(); }
        constexpr explicit operator std::uint16_t() const noexcept
        { return std::uint16_t{this->front()}; }
        constexpr explicit operator std::uint8_t() const noexcept
        { return std::uint8_t{this->front()}; }

        // bool キャスト
        constexpr explicit operator bool() const noexcept
        {
            return static_cast<bool>(this->lower)
                || static_cast<bool>(this->upper);
        }

        // 否定
        constexpr bool operator!() const noexcept
        { return !static_cast<bool>(*this); }

        // ビット反転
        constexpr auto operator~() const noexcept
        {
            auto tmp = fmpint{*this};
            tmp.lower = ~(tmp.lower);
            tmp.upper = ~(tmp.upper);
            return tmp;
        }

        // 左シフト
        constexpr auto& operator<<=(std::size_t n) noexcept
        { return *this = _fmpint_impl::bit_operator(*this).shift_l(n); }

        // 右シフト
        // 算術シフトか、論理シフトかは処理系に準拠
        constexpr auto& operator>>=(std::size_t n) noexcept
        { return *this = _fmpint_impl::bit_operator(*this).shift_r(n); }

        // ビット論理和代入
        constexpr auto& operator|=(const fmpint& v) noexcept
        {
            this->lower |= v.lower;
            this->upper |= v.upper;
            return *this;
        }

        // ビット論理積代入
        constexpr auto& operator&=(const fmpint& v) noexcept
        {
            this->lower &= v.lower;
            this->upper &= v.upper;
            return *this;
        }

        // ビットのxor代入
        constexpr auto& operator^=(const fmpint& v) noexcept
        {
            this->lower ^= v.lower;
            this->upper ^= v.upper;
            return *this;
        }

        // 単項+
        constexpr auto operator+() const noexcept
        { return fmpint{*this}; }

        // 単項-
        constexpr auto operator-() const noexcept
        {
            // 2 の補数を返却
            return ++(~(*this));
        }

        // 加算代入
        constexpr auto& operator+=(const TuIntegral auto& v) noexcept
        { return *this = get_arithmetic(v).add(); }

        // 減算代入
        constexpr auto& operator-=(const TuIntegral auto& v) noexcept
        { return *this += -fmpint{v}; }

        // 乗算代入
        constexpr auto& operator*=(const TuIntegral auto& v) noexcept
        { return *this = get_arithmetic(v).mul(); }

        // 除算代入
        constexpr auto& operator/=(const TuIntegral auto& v)
        { return *this = get_arithmetic(v).div(); }
    
        // 剰余代入
        constexpr auto& operator%=(const TuIntegral auto& v)
        { return *this -= (v * (*this / v)); }

        // 前後インクリメント
        constexpr auto& operator++() noexcept
        { return *this += 1;}
        constexpr auto operator++(int) noexcept
        { return std::exchange(*this, ++fmpint{*this}); }

        // 前後デクリメント
        constexpr auto& operator--() noexcept
        { return *this -= 1;}
        constexpr auto operator--(int) noexcept
        { return std::exchange(*this, --fmpint{*this}); }

        // -------------------------------------------
        // 演算子等のthis適用済み実装クラス取得
        // -------------------------------------------

        constexpr auto get_arithmetic(const TuIntegral auto& v) const noexcept
        { return _fmpint_impl::arithmetic{*this, fmpint{v}}; }

        constexpr auto get_bit_operator() const noexcept
        { return _fmpint_impl::bit_operator{*this}; }
        
        // -------------------------------------------
        // ビット操作メンバ関数
        // -------------------------------------------

        // 指定位置のビットを変更
        constexpr void set_bit(std::size_t i, bool value)
        { *this = get_bit_operator().change_bit(i, value); }

        // -------------------------------------------
        // 内部的な実装
        // -------------------------------------------

        // at の内部表現 (定数ポインタとして this を取らせる)
        // const ポインタかどうかで返却型も変わるはず
        template <class ThisT>
        static constexpr auto& _at(ThisT _this, std::size_t i)
        {
            constexpr auto half_data_length = data_length / 2;
            const auto is_lower = half_data_length > i;
            if constexpr (is_min_size)
                return is_lower ? _this->lower : _this->upper;
            else {
                const auto next_i = i % half_data_length;
                return is_lower
                    ? _this->lower[next_i]
                    : _this->upper[next_i];
            }
        }

        // マイナスかどうか判定
        constexpr bool _is_minus() const noexcept
        { return Signed && get_bit_operator().get_back_bit(); }

        // 内部表現はそのままに符号なし整数へ変換
        constexpr fmpint<Bytes, false> _to_unsigned() const noexcept
        {
            if constexpr (!Signed)
                return fmpint{*this};
            else
                return _switch_sign();
        }

        // 符号ありの時は符号なしに、符号なしの時は符号ありの、内部的表現はそのままに型だけ変換
        constexpr fmpint<Bytes, !Signed> _switch_sign() const noexcept
        {
            fmpint<Bytes, !Signed> new_obj{};
            new_obj.lower = this->lower;
            new_obj.upper = this->upper;
            return new_obj;
        }

        // 比較
        template <bool _Signed>
        constexpr std::strong_ordering _compare(const fmpint<Bytes, _Signed>& v) const noexcept
        {
            constexpr auto inner_compare = [](const half_fmpint& v1, const half_fmpint& v2) {
                if constexpr (is_min_size)
                    return v1 <=> v2;
                else
                    return v1._compare(v2);
            };

            // 異なる符号間の場合、大小関係は自明
            if (const bool is_this_minus = this->_is_minus(); is_this_minus != v._is_minus())
                return is_this_minus
                    ? std::strong_ordering::less
                    : std::strong_ordering::greater;

            // 両方負の場合も、内部的な表現は正の整数と大小関係が同じになるのでそのまま比較実施
            const auto upper_comp = inner_compare(this->upper, v.upper);
            return upper_comp != 0
                ? upper_comp
                : inner_compare(this->lower, v.lower);
        }

        // 10 の n乗をあらかじめ計算しておく
        static constexpr auto _calc_table_10_n()
        {
            using unsigned_t = fmpint<Bytes, false>;
            std::array<unsigned_t, max_digits10 + 1> table{};
            int i = 1;
            table[0] = 1;
            if constexpr (!is_min_size) {
                constexpr auto half_table = half_fmpint::_calc_table_10_n();
                for (; i <= half_fmpint::max_digits10; i++)
                    table[i] = half_table[i];
                for (const auto begin_i = half_fmpint::max_digits10; i <= max_digits10; i++)
                    table[i] = unsigned_t{table[half_fmpint::max_digits10]} *= table[i - begin_i];
            }
            else
                for (; i <= max_digits10; i++)
                    table[i] = unsigned_t{table[i - 1]} * 10;
            return table;
        }

        // 10進数文字列からオブジェクト生成
        static constexpr auto _make_by_digits10_arr(const std::array<int, max_digits10 + 1>& num_arr)
        {
            const auto table_10_n = _calc_table_10_n();
            fmpint new_obj{};
            std::size_t i = 0;
            if constexpr (!is_min_size) {
                std::array<int, half_fmpint::max_digits10 + 1> half_num_arr = {};
                for (int j = 0; auto& num : half_num_arr)
                    num = num_arr[j++];
                // 一つ下のサイズの最大要素はオーバーフローする可能性があるため、
                // 委譲対象外とする
                half_num_arr[i = half_fmpint::max_digits10] = 0;
                new_obj.lower = half_fmpint::_make_by_digits10_arr(half_num_arr);
            }
            for (; i <= max_digits10; i++)
                if (const auto num = num_arr[i]; num > 0)
                    new_obj += (fmpint{table_10_n[i]} *= num);
            return new_obj;
        }

        // 2の累乗進数からオブジェクト生成
        template <std::size_t Base, std::size_t ArrSize = calc_integral_digits_from_bitwidth<Base>(max_digits2)>
        requires (Base == 2 || Base == 8 || Base == 16)
        static constexpr auto _make_by_digits_power2_arr(const std::array<int, ArrSize + 1>& num_arr)
        {
            // n 進数の n ではなく、実際に1桁として表現可能な(n - 1)のビット幅を見なければいけない
            constexpr auto digit_bit_width = std::bit_width(Base - 1);
            fmpint new_obj{};
            for (std::size_t i = 0; auto num : num_arr)
                for (std::size_t j = 0; j < digit_bit_width && i < max_digits2; i++, j++)
                    new_obj.set_bit(i, num & (1 << j));
            return new_obj;
        }

        // 浮動小数点型よりオブジェクト生成
        static constexpr auto _make_by_floating_info(FloatingInfomation auto& v, TuArithmetic auto& original)
        {
            switch (v.get_fpclass()) {
                case FP_INFINITE:
                case FP_NAN:
                    return ~fmpint{};
                case FP_ZERO:
                    return fmpint{};
                default:
                    break;
            }

            fmpint new_obj{};
            if (v.is_normalized() && v.exponent() >= 0) {
                const auto shift_v = v.exponent(false);
                new_obj = shift_v >= 0
                    ? fmpint{v.mantissa()} <<= std::size_t(shift_v)
                    : fmpint{v.mantissa()}.get_bit_operator().shift_r(-shift_v, false);
            }

            // 小数点以下の丸め方法を変換元の浮動小数点に準じるため、
            // 一番小さい桁のみ直接キャストを試みる
            if (v.has_decimal_part()) {
                const auto abs_original = tunum::abs(original);
                new_obj[0] = static_cast<base_data_t>(abs_original);
            }

            return v.sign() < 0
                ? -new_obj
                : new_obj;
        }
    };
}

#endif
