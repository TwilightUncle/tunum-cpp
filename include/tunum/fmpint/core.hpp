#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_CORE_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_CORE_HPP

#include TUNUM_COMMON_INCLUDE(fmpint/meta_function.hpp)
#include TUNUM_COMMON_INCLUDE(utility.hpp)

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
    // TODO: 文字列からの構築時の例外テストを書く
    // TODO: 剰余の演算、ビット反転とマスクでできるんなら計算量へらせね？
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

            if (num_str.starts_with(digits2_prefix_1) || num_str.starts_with(digits2_prefix_2))
                (*this) = _make_by_digits_power2_arr<2>(_make_number_array<2>(num_str));
            else if (num_str.starts_with(digits16_prefix_1) || num_str.starts_with(digits16_prefix_2))
                (*this) = _make_by_digits_power2_arr<16>(_make_number_array<16>(num_str));
            else if (num_str.starts_with(digits8_prefix_1))
                (*this) = _make_by_digits_power2_arr<8>(_make_number_array<8>(num_str));
            else
                (*this) = _make_by_digits10_arr(_make_number_array<10>(num_str));
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
        constexpr const base_data_t& front() const noexcept { return this->at(0); }
        constexpr base_data_t& front() noexcept { return this->at(0); }

        // 内部表現のうち一番大きい値の要素にアクセス
        constexpr const base_data_t& back() const noexcept { return this->at(data_length - 1); }
        constexpr base_data_t& back() noexcept { return this->at(data_length - 1); }

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
        constexpr explicit operator std::uint32_t() const noexcept { return this->front(); }
        constexpr explicit operator std::uint16_t() const noexcept { return std::uint16_t{this->front()}; }
        constexpr explicit operator std::uint8_t() const noexcept { return std::uint8_t{this->front()}; }

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
            const auto fill_mask = ~base_data_t{} << shift_mod;
            for (std::size_t i = 0; i < shift_mul; i++)
                (*this)[i] = 0;
            (*this)[shift_mul] &= fill_mask;
            return *this;
        }

        // 右シフト
        // 算術シフトか、論理シフトかは処理系に準拠
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
            // 算術シフトかつ、最上位ビットが立っている場合、回ってきた部分は1で埋める
            const auto highest_bit = this->get_back_bit();
            const auto fill_mask = ~base_data_t{} << shift_com;
            const auto is_fill_one = is_use_sal && highest_bit;
            for (std::size_t i = 0; i < shift_mul; i++)
                (*this)[data_length - 1 - i] = base_data_t{is_fill_one} * ~base_data_t{};
            if (fill_mask != ~base_data_t{}) {
                if (is_fill_one)
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
                if constexpr (is_min_size)
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
                if constexpr (is_min_size) {
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

        // 先頭ビットを取得
        constexpr bool get_front_bit() const noexcept { return this->get_bit(0); }

        // 最後尾ビットを取得
        constexpr bool get_back_bit() const noexcept { return this->get_bit(max_digits2 - 1); }

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
        constexpr bool _is_minus() const noexcept { return Signed && this->get_back_bit(); }

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

        // 加算
        static constexpr auto _add(const fmpint& v1, const fmpint& v2) noexcept
        {
            constexpr auto inner_add = [](const half_fmpint& l, const half_fmpint& r, bool is_zero_l, bool is_zero_r) -> half_fmpint {
                if (is_zero_r)
                    return l;
                if (is_zero_l)
                    return r;
                if constexpr (is_min_size)
                    return l + r;
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
                if constexpr (is_min_size)
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

            if constexpr (is_min_size)
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
            if constexpr (!is_min_size) {
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

        // 文字を数値へ変換
        template <class CharT>
        static constexpr auto _char_to_num(CharT v)
        {
            using traits_t = std::char_traits<CharT>;
            constexpr CharT code_zero = TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, traits_t, "0")[0];
            constexpr CharT code_a = TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, traits_t, "a")[0];
            constexpr CharT code_A = TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, traits_t, "A")[0];

            if (v >= code_zero && v < code_zero + 10)
                return int(v - code_zero);
            if (v >= code_a && v < code_a + 6)
                return int(v - code_a + 10);
            return int(v - code_A + 10);
        }

        // 進数リテラルの接頭詞を返却
        template <std::size_t Base, class CharT, class Traits = std::char_traits<CharT>>
        static constexpr auto _get_literal_prefixes()
        {
            if constexpr (Base == 2)
                return std::array{
                    TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, Traits, "0b"),
                    TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, Traits, "0B")
                };
            else if constexpr (Base == 8)
                return std::array{
                    TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, Traits, "0")
                };
            else if constexpr (Base == 16)
                return std::array{
                    TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, Traits, "0x"),
                    TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, Traits, "0X")
                };
            else
                return std::array<std::basic_string_view<CharT, Traits>, 0>{};
        }

        // 接頭詞の長さ取得
        template <std::size_t Base>
        static constexpr auto _get_literal_prefix_length() { return (Base == 2 || Base == 16) ? 2 : 0; }

        // 入力文字列が正しいか検証
        template <std::size_t Base, class CharT, class Traits = std::char_traits<CharT>>
        static constexpr bool _valid_input_number_string(std::basic_string_view<CharT, Traits> num_str)
        {
            // 接頭詞を除外した部分の検証
            constexpr auto valid_without_prefix = [](std::basic_string_view<CharT, Traits> _num_str) -> bool {
                constexpr auto double_s_quote = TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, Traits, "''");
                constexpr auto npos = std::basic_string_view<CharT, Traits>::npos;

                // シングルクオテーションから開始する文字列はNG
                if (_num_str.starts_with(double_s_quote[0]))
                    return false;
                // シングルクオテーションが連続して2回以上出現したらNG
                if (_num_str.find(double_s_quote) != npos)
                    return false;
                // 想定外の文字が入力されていないか検査
                for (CharT ch : _num_str)
                    if (ch != double_s_quote[0])
                        if (auto num = _char_to_num(ch); num < 0 || Base <= num)
                            return false;
                return true;
            };

            if constexpr (Base == 2 || Base == 16) {
                // 接頭詞検査のち除去部分の検査
                for (auto pref : _get_literal_prefixes<Base, CharT, Traits>())
                    if (num_str.starts_with(pref))
                        return valid_without_prefix(num_str.substr(_get_literal_prefix_length<Base>()));
            }
            else
                return valid_without_prefix(num_str);
            return false;
        }

        // シングルクオテーションを除去、文字列の並び反転の上、数値配列に変換
        template <
            std::size_t Base,
            class CharT,
            class Traits = std::char_traits<CharT>,
            std::size_t ArrSize = calc_integral_digits_from_bitwidth<Base>(max_digits2)
        >
        static constexpr std::array<int, ArrSize + 1> _make_number_array(std::basic_string_view<CharT, Traits> num_str)
        {
            if (!_valid_input_number_string<Base, CharT, Traits>(num_str))
                throw std::invalid_argument("Specified not number string.");

            num_str = num_str.substr(_get_literal_prefix_length<Base>());
            constexpr CharT s_quote = TUNUM_MAKE_ANY_TYPE_STR_VIEW(CharT, Traits, "'")[0];
            std::array<int, ArrSize + 1> number_array = {};
            // ranges の reverse_view による反転は clang で怒られたため、
            // インデックス操作で読み取り順序を反転させる
            for (int i = 0, ch_i = num_str.length() - 1; ch_i >= 0 && i <= ArrSize; ch_i--)
                if (const auto ch = num_str[ch_i]; ch != s_quote)
                    number_array[i++] = _char_to_num(ch);
            return number_array;
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
    };
}

#endif
