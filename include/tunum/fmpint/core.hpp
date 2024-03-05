#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_CORE_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_CORE_HPP

#include TUNUM_COMMON_INCLUDE(fmpint/meta_function.hpp)

#include <bit>
#include <array>
#include <tuple>
#include <string_view>
#include <stdexcept>
#include <compare>
#include <limits>
#include <numbers>

namespace tunum
{
    // -------------------------------------------
    // クラス実装
    // -------------------------------------------

    // 固定サイズの多倍長整数
    // 内部的な演算方法は組み込みの整数に準拠
    // TODO: 文字列からの構築に10新リテラル以外も選べるようにする
    // TODO: 文字列からの構築時、Ryuあたりのアルゴリズムを用いて効率化できるか調べる
    template <std::size_t Bytes, bool Signed = false>
    struct fmpint
    {
        // -------------------------------------------
        // メンバ定義
        // -------------------------------------------

        // 進数変換等のサイズ計算用
        static constexpr auto log10_2 = std::numbers::ln2 / std::numbers::ln10;

        // 内部的に扱うサイズは2の累乗に統一する
        using base_data_t = std::uint32_t;
        static constexpr std::size_t min_size = sizeof(base_data_t) << 1;
        static constexpr std::size_t size = (std::max)(std::bit_ceil(Bytes), min_size);
        static constexpr std::size_t half_size = size >> 1;
        static constexpr std::size_t data_length = size / sizeof(base_data_t);

        static constexpr std::size_t base_data_digits2 = std::numeric_limits<base_data_t>::digits;
        static constexpr std::size_t max_digits2 = size * 8;
        static constexpr std::size_t max_digits10 = std::size_t(max_digits2 * log10_2);

        using min_size_fmpint = fmpint<(sizeof(base_data_t) << 1), false>;
        using half_fmpint = std::conditional_t<
            half_size == sizeof(base_data_t),
            base_data_t,
            fmpint<half_size, false>
        >;

        half_fmpint upper = {};
        half_fmpint lower = {};

        // -------------------------------------------
        // コンストラクタ
        // -------------------------------------------

        constexpr fmpint() = default;

        // 組み込みの整数から生成
        constexpr fmpint(std::integral auto v) noexcept
            : lower(v)
            , upper((v < 0) ? ~half_fmpint{} : half_fmpint{})
        {
            if constexpr (size == sizeof(v))
                this->upper = std::rotl(v, half_size * 8);
        }

        // 異なるサイズのfmpintから生成(内部表現が等しい)
        template <std::size_t N, bool _Signed>
        requires (Bytes != N && fmpint<N, _Signed>::size == size)
        constexpr fmpint(const fmpint<N, _Signed>& v) noexcept
            : lower(v.lower)
            , upper(v.upper)
        {}

        // 異なるサイズのfmpintから生成(内部表現が小さい)
        template <std::size_t N, bool _Signed>
        requires (fmpint<N, _Signed>::size < size)
        constexpr fmpint(const fmpint<N, _Signed>& v) noexcept
            : lower(v._to_unsigned())
            , upper((v < 0) ? ~half_fmpint{} : half_fmpint{})
        {}

        // 異なるサイズのfmpintから生成
        // 格納できない領域は破棄
        template <std::size_t N, bool _Signed>
        requires (fmpint<N, _Signed>::size > size)
        constexpr fmpint(const fmpint<N, _Signed>& v) noexcept
            : fmpint(v.lower)
        {}

        // c言語風文字列より初期化
        template <class CharT>
        constexpr fmpint(const CharT* num_str) noexcept
            : fmpint(std::basic_string_view<CharT>{num_str})
        {}

        // basic_string_viewあたりを引数として、文字列からの実装を想定
        template <class CharT, class Traits = std::char_traits<CharT>>
        constexpr fmpint(std::basic_string_view<CharT, Traits> num_str) noexcept
        {
            // 文字コードから数値への変換と掛け算実施
            constexpr auto inner_mul = [](auto mul_v, CharT v) {
                constexpr auto char_to_num = [](CharT c, CharT code_zero, CharT code_a, CharT code_A) {
                    if (c >= code_zero && c < code_zero + 10)
                        return int(c - code_zero);
                    if (c >= code_a && c < code_a + 6)
                        return int(c - code_a + 10);
                    return int(c - code_A + 10);
                };

                auto num = 0;
                if constexpr (std::same_as<CharT, wchar_t>)
                    num = char_to_num(v, L'0', L'a', L'A');
                else if constexpr (std::same_as<CharT, char8_t>)
                    num = char_to_num(v, u8'0', u8'a', u8'A');
                else if constexpr (std::same_as<CharT, char16_t>)
                    num = char_to_num(v, u'0', u'a', u'A');
                else if constexpr (std::same_as<CharT, char32_t>)
                    num = char_to_num(v, U'0', U'a', U'A');
                else
                    num = char_to_num(v, '0', 'a', 'A');
                return mul_v *= num;
            };

            const auto table_10_n = _calc_table_10_n();
            auto item = fmpint{1};
            std::size_t i = 0;
            const auto input_digits = num_str.length();
            if constexpr (!std::same_as<base_data_t, half_fmpint>) {
                i = (std::min)(input_digits, half_fmpint::max_digits10);
                const auto substr_begin_pos = input_digits - i;
                    this->lower = half_fmpint{num_str.substr(substr_begin_pos)};
            }
            for (; i < input_digits && i <= max_digits10; i++)
                (*this) += inner_mul(table_10_n[i], num_str[input_digits - 1 - i]);
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

        // -------------------------------------------
        // 演算子オーバーロード
        // -------------------------------------------

        constexpr const base_data_t& operator[](std::size_t n) const { return _at(this, n); }
        constexpr base_data_t& operator[](std::size_t n) { return _at(this, n); }

        // 組み込み整数へのキャスト
        constexpr explicit operator std::uint64_t() const noexcept
        {
            return (std::uint64_t{(*this)[1]} << base_data_digits2)
                | std::uint64_t{(*this)[0]};
        }
        constexpr explicit operator std::uint32_t() const noexcept { return (*this)[0]; }
        constexpr explicit operator std::uint16_t() const noexcept { return std::uint16_t{(*this)[0]}; }
        constexpr explicit operator std::uint8_t() const noexcept { return std::uint8_t{(*this)[0]}; }

        // bool キャスト
        constexpr explicit operator bool() const noexcept
        {
            return static_cast<bool>(this->lower)
                || static_cast<bool>(this->upper);
        }

        // 否定
        constexpr bool operator!() const noexcept { return !static_cast<bool>(*this); }

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
        {
            const auto shift_mod = n % base_data_digits2;
            const auto shift_mul = n / base_data_digits2;

            if (data_length <= shift_mul)
                return *this = fmpint{};
            
            *this = this->rotate_l(n);

            // ローテート演算で回ってきた部分を除去
            for (std::size_t i = 0; i < shift_mul; i++)
                (*this)[i] = 0;
            const auto fill_mask = ~base_data_t{} << shift_mod;
            (*this)[shift_mul] &= fill_mask;
            return *this;
        }

        // 右シフト
        // 算術シフトか、論理シフトかは処理系の処理に準拠
        constexpr auto& operator>>=(std::size_t n) noexcept
        {
            // 組み込み整数の右シフトが算術シフトとして実装されているか判定
            // ※最上ビットが1の状態で、右シフト後の最上位ビットが1であれば算術シフト、そうでなければ論理シフト
            constexpr auto is_use_sal = Signed
                ? ((-1) >> 1) == -1
                : ((~0u) >> 1) == (~0u);

            const auto shift_mod = n % base_data_digits2;
            const auto shift_com = (base_data_digits2 - shift_mod) % base_data_digits2;
            const auto shift_mul = n / base_data_digits2;

            if (data_length <= shift_mul)
                return *this = fmpint{};
            
            *this = this->rotate_r(n);

            // ローテート演算で回ってきた部分を除去
            for (std::size_t i = 0; i < shift_mul; i++)
                (*this)[data_length - 1 - i] = 0;
            const auto highest_bit = this->get_bit(max_digits2 - 1);
            const auto fill_mask = ~base_data_t{} << shift_com;
            if (fill_mask != ~base_data_t{}) {
                if (highest_bit && is_use_sal)
                    // 算術シフトかつ、最上位ビットが立っている場合、回ってきた部分は1で埋める
                    (*this)[shift_mul] ^= fill_mask;
                else
                    (*this)[shift_mul] &= ~fill_mask;
            }
            return *this;
        }

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
        constexpr auto operator+() const noexcept { return fmpint{*this}; }

        // 単項-
        constexpr auto operator-() const noexcept
        {
            // 2 の補数を返却
            return ++(~(*this));
        }

        // 加算代入
        constexpr auto& operator+=(const TuIntegral auto& v) noexcept { return *this = _add(*this, fmpint{v}); }

        // 減算代入
        constexpr auto& operator-=(const TuIntegral auto& v) noexcept { return *this += -fmpint{v}; }

        // 乗算代入
        constexpr auto& operator*=(const TuIntegral auto& v) noexcept { return *this = _mul(*this, fmpint{v}); }

        // 除算代入
        constexpr auto& operator/=(const TuIntegral auto& v) { return *this = _div(*this, fmpint{v})[0]; }
    
        // 剰余代入
        constexpr auto& operator%=(const TuIntegral auto& v) { return *this = _div(*this, fmpint{v})[1]; }

        // 前後インクリメント
        constexpr auto& operator++() noexcept { return *this += 1;}
        constexpr auto operator++(int) noexcept { return std::exchange(*this, ++fmpint{*this}); }

        // 前後デクリメント
        constexpr auto& operator--() noexcept { return *this -= 1;}
        constexpr auto operator--(int) noexcept { return std::exchange(*this, --fmpint{*this}); }

        // -------------------------------------------
        // ビット操作メンバ関数
        // -------------------------------------------

        // ビット左ローテーション
        constexpr auto rotate_l(std::size_t n) const noexcept
        {
            auto this_clone = fmpint{*this};
            if (!n) return this_clone;

            const auto shift_mod = n % base_data_digits2;
            const auto i_diff = n / base_data_digits2;
            const auto shift_com = (base_data_digits2 - shift_mod) % base_data_digits2;

            for (std::size_t i = 0; i < data_length; i++) {
                auto& elem = this_clone[(i + i_diff) % data_length];
                elem = ((*this)[i] << shift_mod);
                if (shift_com)
                    elem |= ((*this)[(i + data_length - 1) % data_length] >> shift_com);
            }
            return this_clone;
        }

        // ビット右ローテーション
        constexpr auto rotate_r(std::size_t n) const noexcept { return this->rotate_l(max_digits2 - (n % max_digits2)); }

        // 左側に連続している 0 ビットの数を返却
        constexpr auto countl_zero_bit() const noexcept { return this->count_continuous_bit(false, true); }

        // 左側に連続している 1 ビットの数を返却
        constexpr auto countl_one_bit() const noexcept { return this->count_continuous_bit(true, true); }

        // 右側に連続している 0 ビットの数を返却
        constexpr auto countr_zero_bit() const noexcept { return this->count_continuous_bit(false, false); }

        // 右側に連続している 1 ビットの数を返却
        constexpr auto countr_one_bit() const noexcept { return this->count_continuous_bit(true, false); }

        // 寝ているビットをカウント
        constexpr auto count_zero_bit() const noexcept { return (~(*this)).count_one_bit(); }

        // 立っているビットをカウント
        constexpr auto count_one_bit() const noexcept
        {
            constexpr auto inner_count = [](const half_fmpint& v) {
                if constexpr (std::integral<half_fmpint>)
                    return std::popcount(v);
                else
                    return v.count_one_bit();
            };
            return inner_count(this->lower) + inner_count(this->upper);
        }

        // 全てのビットが立っているかどうか判定。
        // 引数にfalseを指定すると、すべてのビットが寝ているかどうか判定
        constexpr bool is_full_bit(const bool bit = true) const noexcept
        {
            return bit
                ? this->count_zero_bit() == 0
                : this->count_one_bit() == 0;
        }

        // 指定されたビットが指定の方向(左右)から連続でいくつ並んでいるかかカウント
        constexpr auto count_continuous_bit(bool bit, bool is_begin_l) const noexcept
        {
            constexpr auto inner_f = [](const half_fmpint& v, bool _bit, bool _is_begin_l) {
                if constexpr (std::integral<half_fmpint>) {
                    // ビット反転すると結果は同じでしょう
                    const auto _v = !_bit ? ~v : v;
                    return _is_begin_l ? std::countl_one(_v) : std::countr_one(_v);
                }
                else
                    return v.count_continuous_bit(_bit, _is_begin_l);
            };

            const auto first_bit_cnt = inner_f(is_begin_l ? this->upper : this->lower, bit, is_begin_l); 
            // フルビットじゃなければ連続していないのでその場で返却
            return first_bit_cnt != (base_data_digits2 * data_length / 2)
                ? first_bit_cnt
                : first_bit_cnt + inner_f(is_begin_l ? this->lower : this->upper, bit, is_begin_l);
        }

        // 格納値を表現するのに必要なビット幅を返却
        constexpr auto get_bit_width() const noexcept { return max_digits2 - this->countl_zero_bit(); }

        // 指定位置のビットを取得
        constexpr bool get_bit(std::size_t i) const
        {
            const auto base_data_index = i / base_data_digits2;
            const auto bit_pos = i % base_data_digits2;
            return bool(this->at(base_data_index) & (base_data_t{1} << bit_pos));
        }

        // 指定位置のビットを変更
        constexpr void set_bit(std::size_t i, bool value)
        {
            const auto base_data_index = i / base_data_digits2;
            const auto bit_pos = i % base_data_digits2;
            const auto single_bit = base_data_t{1} << bit_pos;
            if (!value)
                this->at(base_data_index) &= (~single_bit);
            else
                this->at(base_data_index) |= single_bit;
        }

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
            if constexpr (std::integral<half_fmpint>)
                return is_lower ? _this->lower : _this->upper;
            else {
                const auto next_i = i % half_data_length;
                return is_lower
                    ? _this->lower[next_i]
                    : _this->upper[next_i];
            }
        }

        // マイナスかどうか判定
        constexpr bool _is_minus() const noexcept { return Signed && this->get_bit(max_digits2 - 1); }

        // 内部表現はそのままに符号なし整数へ変換
        constexpr fmpint<Bytes, false> _to_unsigned() const noexcept
        {
            fmpint<Bytes, false> new_obj{};
            new_obj.lower = this->lower;
            new_obj.upper = this->upper;
            return new_obj;
        }

        // 比較
        template <bool _Signed>
        constexpr std::strong_ordering _compare(const fmpint<Bytes, _Signed>& v) const noexcept
        {
            constexpr auto inner_compare = [](const half_fmpint& v1, const half_fmpint& v2) {
                if constexpr (std::same_as<half_fmpint, base_data_t>)
                    return v1 <=> v2;
                else
                    return v1._compare(v2);
            };

            // いずれかが負の値であれば、ビット反転したうえで大小比較
            if (this->_is_minus() || v._is_minus())
                return (~this->_to_unsigned())._compare(~v._to_unsigned());

            const auto upper_comp = inner_compare(this->upper, v.upper);
            return upper_comp != 0
                ? upper_comp
                : inner_compare(this->lower, v.lower);
        }

        // 加算
        static constexpr auto _add(const fmpint& v1, const fmpint& v2) noexcept
        {
            constexpr auto inner_add = [](const half_fmpint& l, const half_fmpint& r, bool is_zero_l, bool is_zero_r) -> half_fmpint {
                if (is_zero_r)
                    return l;
                if (is_zero_l)
                    return r;
                if constexpr (std::same_as<base_data_t, half_fmpint>)
                    return std::uint64_t(l) + std::uint64_t(r);
                else
                    return half_fmpint::_add(l, r);
            };

            const auto is_zero_v1_l = !v1.lower;
            const auto is_zero_v1_u = !v1.upper;
            const auto is_zero_v2_l = !v2.lower;
            const auto is_zero_v2_u = !v2.upper;
            if (is_zero_v1_l && is_zero_v2_u || is_zero_v1_u && is_zero_v2_l)
                return fmpint{v1} |= v2;

            // 2 桁の筆算のような感じ
            auto r = fmpint{};
            r.lower = inner_add(v1.lower, v2.lower, is_zero_v1_l, is_zero_v2_l);
            r.upper = inner_add(v1.upper, v2.upper, is_zero_v1_u, is_zero_v2_u);

            // 桁上りがある場合
            if ((~v1.lower) < v2.lower)
                r.upper = inner_add(r.upper, 1, !r.upper, false);
            return r;
        }

        // 乗算
        static constexpr auto _mul(const fmpint& v1, const fmpint& v2) noexcept
        {
            using next_size_fmpint = fmpint<(size << 1)>;
            constexpr auto inner_mul = [](const half_fmpint& l, const half_fmpint& r) {
                if constexpr (std::same_as<base_data_t, half_fmpint>)
                    return fmpint{std::uint64_t(l) * std::uint64_t(r)};
                else
                    return half_fmpint::_mul(l, r);
            };

            const auto is_zero_v1_l = !v1.lower;
            const auto is_zero_v1_u = !v1.upper;
            const auto is_zero_v2_l = !v2.lower;
            const auto is_zero_v2_u = !v2.upper;
            if (is_zero_v1_l && is_zero_v1_u || is_zero_v2_l && is_zero_v2_u)
                return next_size_fmpint{0};

            // たすき掛け
            auto r = next_size_fmpint{};
            if (!is_zero_v1_l && !is_zero_v2_l)
                r.lower = inner_mul(v1.lower, v2.lower);
            if (!is_zero_v1_u && !is_zero_v2_u)
                r.upper = inner_mul(v1.upper, v2.upper);
            // 下の桁と上の桁をクロスする部分
            auto r2 = next_size_fmpint{};
            if (!is_zero_v1_u && !is_zero_v2_l)
                r2.lower = inner_mul(v1.upper, v2.lower);
            if (!is_zero_v1_l && !is_zero_v2_u)
                r2 += inner_mul(v1.lower, v2.upper);
            return r += (r2 <<= (size * 8 / 2));
        }

        // 割り算の商と余りの2要素配列を返却
        // v1 と v2の差が大きいほど計算量も増える
        static constexpr auto _div(const fmpint& v1, const fmpint& v2)
        {
            // 重いので計算せずとも自明なものはここではじいておく
            if (!v2)
                throw std::invalid_argument{"0 div."};
            if (!v1)
                return std::array{fmpint{}, fmpint{}};
            if (v1 < v2)
                return std::array{fmpint{}, fmpint{v1}};
            if (v2 == 1)
                return std::array{fmpint{v1}, fmpint{}};

            if constexpr (std::same_as<base_data_t, half_fmpint>)
                // 組み込みの演算子使えるならそっち優先
                return std::array{
                    fmpint{std::uint64_t{v1} / std::uint64_t{v2}},
                    fmpint{std::uint64_t{v1} % std::uint64_t{v2}}
                };
            else {
                // 両側の0ビットを除去したビット幅がより小さい型でも計算可能な際はそちらへ処理を委譲
                // ※約数みたいなもんで、計算回数を減らす
                if (
                    const auto min_zero_r_cnt = (std::min)(v1.countr_zero_bit(), v2.countr_zero_bit());
                    max_digits2 - (v1.countl_zero_bit() + min_zero_r_cnt) <= max_digits2 / 2
                ) {
                    const auto [_quo, _rem] = half_fmpint::_div(
                        (fmpint{v1} >>= min_zero_r_cnt).lower,
                        (fmpint{v2} >>= min_zero_r_cnt).lower
                    );
                    // 余りは桁を削ってはいけないので戻す
                    return std::array{fmpint{_quo}, fmpint{_rem} <<= min_zero_r_cnt};
                }

                // v1 と v2 の２進数桁数の差より、v2のシフト数を取得
                const std::size_t v2_lshift_cnt = v1.get_bit_width() - v2.get_bit_width();
                auto rem = fmpint{v1};
                auto quo = fmpint{};
                for (std::size_t i = 0; i <= v2_lshift_cnt; i++) {
                    const auto shifted_v2 = fmpint{v2} <<= (v2_lshift_cnt - i);
                    if (rem >= shifted_v2) {
                        quo.set_bit(v2_lshift_cnt - i, true);
                        rem -= shifted_v2;
                    }
                }
                return std::array{quo, rem};
            }
        }

        // 10 の n乗をあらかじめ計算しておく
        static constexpr auto _calc_table_10_n()
        {
            std::array<fmpint, max_digits10 + 1> table{};
            int i = 1;
            table[0] = 1;
            if constexpr (!std::same_as<base_data_t, half_fmpint>) {
                constexpr auto half_table = half_fmpint::_calc_table_10_n();
                for (; i <= half_fmpint::max_digits10; i++)
                    table[i] = half_table[i];
                for (const auto begin_i = half_fmpint::max_digits10; i <= max_digits10; i++)
                    table[i] = fmpint{table[half_fmpint::max_digits10]} *= table[i - begin_i];
            }
            else
                for (; i <= max_digits10; i++)
                    table[i] = fmpint{table[i - 1]} * 10;
            return table;
        }
    };
}

#endif
