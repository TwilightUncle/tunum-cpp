#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_HPP

#include <bit>
#include <bitset>
#include <concepts>
#include <algorithm>
#include <array>
#include <string_view>
#include <ranges>
#include <stdexcept>
#include <vector>
#include <compare>

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

        // 内部的に扱うサイズは2の累乗に統一する
        using base_data_t = std::uint32_t;
        static constexpr std::size_t min_size = sizeof(base_data_t) << 1;
        static constexpr std::size_t size = (std::max)(std::bit_ceil(Bytes), min_size);
        static constexpr std::size_t half_size = size >> 1;
        static constexpr std::size_t data_length = size / sizeof(base_data_t);

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

        // basic_string_viewあたりを引数として、文字列からの実装を想定
        // template <class CharT, class Traits = std::char_traits<CharT>>
        // constexpr fmpint()

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

        // bool キャスト
        explicit operator bool() const noexcept
        {
            return static_cast<bool>(this->lower)
                || static_cast<bool>(this->upper);
        }

        // 否定
        bool operator!() const noexcept { return !static_cast<bool>(*this); }

        // ビット反転
        constexpr auto operator~() const noexcept
        {
            auto tmp = fmpint{*this};
            tmp.lower = ~(tmp.lower);
            tmp.upper = ~(tmp.upper);
            return tmp;
        }

        // 左シフト
        constexpr auto operator<<=(std::size_t n) noexcept
        {
            const auto shift_mod = n % (sizeof(base_data_t) * 8);
            const auto shift_com = (sizeof(base_data_t) * 8) - shift_mod;
            const auto shift_mul = n / (sizeof(base_data_t) * 8);

            if (data_length <= shift_mul)
                return *this = fmpint{};
            
            this->rotate_l(n);
            for (std::size_t i = 0; i< shift_mul; i++)
                (*this)[i] = 0;

            // ローテート演算で回ってきた部分を除去
            ((*this)[shift_mul] >>= shift_com) <<= shift_com;
            return *this;
        }

        // 右シフト
        constexpr auto operator>>=(std::size_t n) noexcept
        {
            const auto shift_mod = n % (sizeof(base_data_t) * 8);
            const auto shift_com = (sizeof(base_data_t) * 8) - shift_mod;
            const auto shift_mul = n / (sizeof(base_data_t) * 8);

            if (data_length <= shift_mul)
                return *this = fmpint{};
            
            this->rotate_r(n);
            for (std::size_t i = shift_mul + 1; i < data_length; i++)
                (*this)[i] = 0;

            // ローテート演算で回ってきた部分を除去
            ((*this)[shift_mul] <<= shift_com) >>= shift_com;
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
            return (~(*this)) += 1;
        }

        // 加算代入
        constexpr auto& operator+=(const TuIntegral auto& v) noexcept { return *this = _add(*this, fmpint{v}); }

        // 減算代入
        constexpr auto& operator-=(const TuIntegral auto& v) noexcept { return *this += -v; }

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
        // その他メンバ関数
        // -------------------------------------------

        // ビット左ローテーション
        constexpr auto rotate_l(std::size_t n) noexcept
        {
            const auto shift_mod = n % (sizeof(base_data_t) * 8);
            const auto shift_com = (sizeof(base_data_t) * 8) - shift_mod;
            const auto this_clone = fmpint{*this};
            for (std::size_t i = 0; i < data_length; i++)
                (*this)[i] = (this_clone[i] << shift_mod)
                    | (this_clone[i + data_length - 1 % data_length] >> shift_com);
            return *this;
        }

        // ビット右ローテーション
        constexpr auto rotate_r(std::size_t n) noexcept
        {
            const auto shift_mod = n % (sizeof(base_data_t) * 8);
            const auto shift_com = (sizeof(base_data_t) * 8) - shift_mod;
            return this->rotate_l(n - shift_mod + shift_com);
        }

        // 左側に連続している 0 ビットの数を返却
        constexpr auto countl_zero() const noexcept
        {
            for (int cnt{}, i = data_length - 1; i >= 0; i--, cnt += sizeof(base_data_t) * 8)
                if ((*this)[i])
                    return cnt + std::countl_zero((*this)[i]);
            return size * 8;
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
        template <std::size_t Bytes2>
        constexpr auto _compare(const fmpint<Bytes2>& v) const noexcept
        {
            using arg_t = fmpint<Bytes2>;
            if constexpr (size < arg_t::size)
                return arg_t{*this}._compare(v);
            else {
                constexpr auto inner_compare = [](const half_fmpint& v1, const half_fmpint& v2) {
                    if constexpr (std::same_as<half_fmpint, base_data_t>)
                        return v1 <=> v2;
                    else return v1._compare(v2);
                };

                const auto resized_v = fmpint{v};
                const auto upper_comp = inner_compare(v.upper, resized_v.upper);
                return upper_comp != 0
                    ? upper_comp
                    : inner_compare(v.lower, resized_v.lower);
            }
        }

        // 加算の際にデータの損失が起こらないよう、一つ上のサイズの整数にして返却
        static constexpr auto _add(const fmpint& v1, const fmpint& v2) noexcept
        {
            using next_size_fmpint = fmpint<(size << 1)>;
            using self_size_fmpint = fmpint<size>;

            // 型の形式の違いを吸収
            constexpr auto inner_add = [](const half_fmpint& l, const half_fmpint& r) {
                if constexpr (std::same_as<base_data_t, half_fmpint>)
                    return self_size_fmpint{std::uint64_t(l) + std::uint64_t(r)};
                else
                    return half_fmpint::_add(l, r);
            };

            // 2 桁の筆算のような感じ
            const auto l = inner_add(v1.lower, v2.lower);
            const auto u = inner_add(v1.upper, v2.upper);
            const auto carry = inner_add(l.upper, u.lower);
            auto next_lower = self_size_fmpint{l.lower};
            next_lower.upper = carry.lower;
            auto r = next_size_fmpint{next_lower};
            r.upper = inner_add(u.upper, carry.upper);
            return r;
        }

        // 乗算
        static constexpr auto _mul(const fmpint& v1, const fmpint& v2) noexcept
        {
            using next_size_fmpint = fmpint<(size << 1)>;
            using self_size_fmpint = fmpint<size>;
            constexpr auto inner_mul = [](const half_fmpint& l, const half_fmpint& r) {
                if constexpr (std::same_as<base_data_t, half_fmpint>)
                    return self_size_fmpint{std::uint64_t(l) * std::uint64_t(r)};
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
        static constexpr auto _div(const fmpint& v1, const fmpint& v2)
        {
            if (!v2) throw std::invalid_argument{"0 div."};
            if (!v1) return std::array{fmpint{}, fmpint{}};
            if (v1._compare(v2) < 0)
                return std::array{fmpint{}, fmpint{v1}};
            if (v2._compare(fmpint{1}) == 0)
                return std::array{fmpint{v1}, fmpint{}};

            // v1 と v2 の先頭ビット位置より、v2のシフト数を取得
            std::size_t v2_lshift_cnt = (std::max)(v2.countl_zero() - v1.countl_zero(), 0);
            auto clone_v2 = fmpint{v2} <<= v2_lshift_cnt;

            // コア部分
            auto rem = fmpint{v1};
            auto quo = fmpint{};
            while ((clone_v2)._compare(v2) >= 0) {
                if (rem._compare(clone_v2) >= 0) {
                    quo |= (fmpint{1} <<= v2_lshift_cnt);
                    rem -= clone_v2;
                }
                clone_v2 >>= 1;
                v2_lshift_cnt--;
            }
            return std::array{quo, rem};
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

    constexpr auto operator<=>(const TuFmpIntegral auto& l, const TuIntegral auto& r) { return l._compare(r); }
    template <TuFmpIntegral T>
    constexpr auto operator<=>(std::integral auto l, const T& r) { return T{l}._compare(r); }

    TUNUM_FUNC_MAKE_FMPINT_OPERATOR(operator+, +=)

    TUNUM_FUNC_MAKE_FMPINT_OPERATOR(operator-, -=)

    TUNUM_FUNC_MAKE_FMPINT_OPERATOR(operator*, *=)

    TUNUM_FUNC_MAKE_FMPINT_OPERATOR(operator/, /=)

#undef TUNUM_FUNC_MAKE_FMPINT_OPERATOR

    // -------------------------------------------
    // エイリアス定義
    // -------------------------------------------

    using int128_t = fmpint<(sizeof(std::uint64_t) << 1)>;
    using int256_t = fmpint<(sizeof(std::uint64_t) << 2)>;
    using int512_t = fmpint<(sizeof(std::uint64_t) << 3)>;
}

#endif
