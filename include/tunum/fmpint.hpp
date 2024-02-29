#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_HPP

#include <bit>
#include <concepts>
#include <array>
#include <tuple>
#include <string_view>
#include <stdexcept>
#include <compare>
#include <limits>
#include <numbers>

namespace tunum
{
    template <std::size_t Bytes>
    struct fmpint;

    // -------------------------------------------
    // メタ関数・コンセプト
    // -------------------------------------------

    // fmpint かどうか判定
    template <class T>
    struct is_fmpint : public std::false_type {};
    template <std::size_t Bytes>
    struct is_fmpint<fmpint<Bytes>> : public std::true_type {};

    // fmpint かどうか判定
    template <class T>
    constexpr bool is_fmpint_v = is_fmpint<T>::value;

    // fmpint かどうか判定
    template <class T>
    concept TuFmpIntegral = is_fmpint_v<T>;

    // fmpint または 組み込み整数型 かどうか判定
    template <class T>
    concept TuIntegral = is_fmpint_v<T> || std::is_integral_v<T>;

    // 制約 TuIntegral が真となる型の byte サイズを取得
    template <class T>
    struct get_integral_size : public std::integral_constant<int, -1> {};
    template <std::integral T>
    struct get_integral_size<T> : public std::integral_constant<int, sizeof(T)> {};
    template <std::size_t Bytes>
    struct get_integral_size<fmpint<Bytes>> : public std::integral_constant<int, fmpint<Bytes>::size> {};

    // 制約 TuIntegral が真となる型の byte サイズを取得
    template <class T>
    constexpr int get_integral_size_v = get_integral_size<T>::value;

    // 制約 TuIntegral が真となる二つの型のうち、大きいほうと同じサイズの fmpint を返却
    template <TuIntegral T1, TuIntegral T2>
    struct get_large_integral : public std::type_identity<
        fmpint<(std::max)(
            get_integral_size_v<T1>,
            get_integral_size_v<T2>
        )>
    > {};

    // 制約 TuIntegral が真となる二つの型のうち、大きいほうと同じサイズの fmpint を返却
    template <TuIntegral T1, TuIntegral T2>
    using get_large_integral_t = typename get_large_integral<T1, T2>::type;

    // -------------------------------------------
    // クラス実装
    // -------------------------------------------

    // 固定サイズの多倍長整数
    // 内部的な演算方法は組み込みの整数に準拠
    template <std::size_t Bytes>
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
        static constexpr std::size_t max_digits10 = std::size_t(max_digits2 * log10_2) + 1;

        using min_size_fmpint = fmpint<(sizeof(base_data_t) << 1)>;
        using half_fmpint = std::conditional_t<
            half_size == sizeof(base_data_t),
            base_data_t,
            fmpint<half_size>
        >;

        half_fmpint upper = {};
        half_fmpint lower = {};

        // -------------------------------------------
        // コンストラクタ
        // -------------------------------------------

        constexpr fmpint() = default;

        // 組み込みの整数から生成
        constexpr fmpint(std::integral auto v) noexcept
            : lower(v < 0 ? -v : v)
        {
            if constexpr (size == sizeof(v))
                this->upper = std::rotl(v < 0 ? -v : v, half_size * 8);
        }

        // 異なるサイズのfmpintから生成(内部表がんが等しいか小さいもの)
        template <std::size_t N>
        requires (N != Bytes && N <= size)
        constexpr fmpint(const fmpint<N>& v) noexcept
        {
            constexpr std::size_t src_size = fmpint<N>::size;
            if constexpr (size == src_size) {
                // 内部表現が同じ場合
                this->upper = v.upper;
                this->lower = v.lower;
            }
            else this->lower = half_fmpint{v};
        }

        // 異なるサイズのfmpintから生成
        // 格納できない領域は破棄
        template <std::size_t N>
        requires (N > size)
        constexpr fmpint(const fmpint<N>& v) noexcept
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
            // 保存領域の境界にまたがる可能性のある10進数の桁かどうか判定
            constexpr auto get_bound = [](const std::size_t digits) {
                for (std::size_t i = 0; i < data_length; i++) {
                    const auto bound = std::size_t(i * base_data_digits2 * log10_2) + 1;
                    if (bound >= digits)
                        return std::pair{i && bound == digits, i};
                }
                return std::pair{false, std::size_t{}};
            };

            constexpr auto char_to_num = [](CharT c, CharT code_zero, CharT code_a, CharT code_A) {
                if (c >= code_zero && c < code_zero + 10)
                    return int(c - code_zero);
                if (c >= code_a && c < code_a + 6)
                    return int(c - code_a + 10);
                return int(c - code_A + 10);
            };

            // 文字コードから数値への変換と掛け算実施
            constexpr auto inner_mul = [&char_to_num](auto mul_v, CharT v) {
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
    
            base_data_t item = 1;
            for (std::size_t i = 0; i < num_str.length(); i++) {
                const auto digits = i + 1;
                const auto [is_bound, buf_i] = get_bound(digits);
                if (max_digits10 < digits)
                    break;
                if (is_bound) {
                    auto large_item = fmpint{};
                    large_item[buf_i - 1] = item;
                    (*this) += inner_mul(large_item, num_str[i]);
                    item = 1;
                }
                else {
                    (*this)[buf_i] += inner_mul(item, num_str[i]);
                    item *= 10;
                }
            }
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
            (*this)[shift_mul] >>= shift_mod;
            (*this)[shift_mul] <<= shift_mod;
            return *this;
        }

        // 右シフト
        constexpr auto& operator>>=(std::size_t n) noexcept
        {
            const auto shift_mod = n % base_data_digits2;
            const auto shift_mul = n / base_data_digits2;

            if (data_length <= shift_mul)
                return *this = fmpint{};
            
            *this = this->rotate_r(n);

            // ローテート演算で回ってきた部分を除去
            for (std::size_t i = 0; i < shift_mul; i++)
                (*this)[data_length - 1 - i] = 0;
            (*this)[shift_mul] <<= shift_mod;
            (*this)[shift_mul] >>= shift_mod;
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
        constexpr auto get_bit(std::size_t i) const
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

        // 比較
        constexpr auto _compare(const fmpint& v) const noexcept
        {
            constexpr auto inner_compare = [](const half_fmpint& v1, const half_fmpint& v2) {
                if constexpr (std::same_as<half_fmpint, base_data_t>)
                    return v1 <=> v2;
                else return v1._compare(v2);
            };

            const auto upper_comp = inner_compare(this->upper, v.upper);
            return upper_comp != 0
                ? upper_comp
                : inner_compare(this->lower, v.lower);
        }

        // 加算の際にデータの損失が起こらないよう、一つ上のサイズの整数にして返却
        static constexpr auto _add(const fmpint& v1, const fmpint& v2) noexcept
        {
            using next_size_fmpint = fmpint<(size << 1)>;
            constexpr auto inner_add = [](const half_fmpint& l, const half_fmpint& r) {
                if constexpr (std::same_as<base_data_t, half_fmpint>)
                    return fmpint{std::uint64_t(l) + std::uint64_t(r)};
                else
                    return half_fmpint::_add(l, r);
            };

            // 2 桁の筆算のような感じ
            const auto l = inner_add(v1.lower, v2.lower);
            const auto u = inner_add(v1.upper, v2.upper);
            const auto carry = inner_add(l.upper, u.lower);
            auto next_lower = fmpint{l.lower};
            next_lower.upper = carry.lower;
            auto r = next_size_fmpint{next_lower};
            r.upper = inner_add(u.upper, carry.upper);
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

            // たすき掛け
            return next_size_fmpint{inner_mul(v1.lower, v2.lower)}
                += (next_size_fmpint{inner_mul(v1.upper, v2.lower)} <<= (size * 8 / 2))
                += (next_size_fmpint{inner_mul(v1.lower, v2.upper)} <<= (size * 8 / 2))
                += (next_size_fmpint{inner_mul(v1.upper, v2.upper)} <<= (size * 8));
        }

        // 割り算の商と余りのペアを返却
        // v1 と v2の差が大きいほど計算量も増える
        static constexpr auto _div(const fmpint& v1, const fmpint& v2)
        {
            // 重いので計算せずとも自明なものはここではじいておく
            if (!v2) throw std::invalid_argument{"0 div."};
            if (!v1) return std::array{fmpint{}, fmpint{}};
            if (v1._compare(v2) < 0)
                return std::array{fmpint{}, fmpint{v1}};
            if (v2._compare(fmpint{1}) == 0)
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
                    if (rem._compare(shifted_v2) >= 0) {
                        quo.set_bit(v2_lshift_cnt - i, true);
                        rem -= shifted_v2;
                    }
                }
                return std::array{quo, rem};
            }
        }
    };

    // -------------------------------------------
    // 演算子オーバーロード(グローバル)
    // -------------------------------------------

#ifndef TUNUM_FUNC_MAKE_FMPINT_OPERATOR
#define TUNUM_FUNC_MAKE_FMPINT_OPERATOR(op_name, op) template <TuFmpIntegral T1, TuIntegral T2> \
    constexpr auto op_name(const T1& l, const T2& r) { return get_large_integral_t<T1, T2>{l} op r; } \
    template <TuFmpIntegral T> \
    constexpr auto op_name(std::integral auto l, const T&  r) { return T{l} op r; }
#endif

    template <TuFmpIntegral T>
    constexpr auto operator<<(const T& l, std::size_t r) { return T{l} <<= r; }

    template <TuFmpIntegral T>
    constexpr auto operator>>(const T& l, std::size_t r) { return T{l} >>= r; }

    TUNUM_FUNC_MAKE_FMPINT_OPERATOR(operator|, |=)
    TUNUM_FUNC_MAKE_FMPINT_OPERATOR(operator&, &=)
    TUNUM_FUNC_MAKE_FMPINT_OPERATOR(operator^, ^=)

    template <TuFmpIntegral T1, TuIntegral T2>
    constexpr auto operator<=>(const T1& l, const T2& r)
    {
        using large_integral_t = get_large_integral_t<T1, T2>;
        return large_integral_t{l}._compare(large_integral_t{r});
    }
    template <TuFmpIntegral T>
    constexpr auto operator<=>(std::integral auto l, const T& r) { return T{l}._compare(r); }

    constexpr bool operator==(const TuFmpIntegral auto& l, const TuIntegral auto& r) { return (l <=> r) == 0; }
    constexpr bool operator==(std::integral auto l, const TuFmpIntegral auto& r) { return (l <=> r) == 0; }

    TUNUM_FUNC_MAKE_FMPINT_OPERATOR(operator+, +=)
    TUNUM_FUNC_MAKE_FMPINT_OPERATOR(operator-, -=)
    TUNUM_FUNC_MAKE_FMPINT_OPERATOR(operator*, *=)
    TUNUM_FUNC_MAKE_FMPINT_OPERATOR(operator/, /=)
    TUNUM_FUNC_MAKE_FMPINT_OPERATOR(operator%, %=)

#undef TUNUM_FUNC_MAKE_FMPINT_OPERATOR

    // -------------------------------------------
    // エイリアス定義
    // -------------------------------------------

    using int128_t = fmpint<(sizeof(std::uint64_t) << 1)>;
    using int256_t = fmpint<(sizeof(std::uint64_t) << 2)>;
    using int512_t = fmpint<(sizeof(std::uint64_t) << 3)>;
}

#endif
