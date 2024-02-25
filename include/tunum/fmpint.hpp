#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_HPP

#include <bit>
#include <bitset>
#include <concepts>
#include <algorithm>
#include <array>
#include <ranges>
#include <stdexcept>

namespace tunum
{
    // 固定サイズの多倍長整数
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

        using min_size_fmpint = fmpint<(sizeof(base_data_t) << 1)>;
        using half_fmpint = std::conditional_t<
            half_size == sizeof(base_data_t),
            base_data_t,
            fmpint<half_size>
        >;

        bool sign = {};
        half_fmpint upper = {};
        half_fmpint lower = {};

        // -------------------------------------------
        // コンストラクタ
        // -------------------------------------------

        constexpr fmpint() = default;

        // 組み込みの整数から生成
        constexpr fmpint(std::integral auto v) noexcept
            : sign(v < 0)
            , lower(v < 0 ? -v : v)
        {
            if constexpr (size == sizeof(v))
                this->upper = std::rotl(v < 0 ? -v : v, half_size * 8);
        }

        // 異なるサイズのfmpintから生成(内部表がんが等しいか小さいもの)
        template <std::size_t N>
        requires (N != Bytes && N <= size)
        constexpr fmpint(const fmpint<N>& v) noexcept
            : sign(v.sign)
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
        {
            this->sign = v.sign;
        }

        // -------------------------------------------
        // 要素アクセス
        // -------------------------------------------

        // 内部表現へのアクセス
        constexpr base_data_t at(int i) const
        {
            constexpr auto data_size = sizeof(base_data_t);
            constexpr auto data_length = size / data_size;
            constexpr auto half_data_length = data_length / 2;
            if (i >= data_length) throw std::out_of_range{""};

            const auto is_lower = half_data_length > i;
            if constexpr (std::integral<half_fmpint>)
                return is_lower ? this->lower : this->upper;
            else {
                const int next_i = i % half_data_length;
                return is_lower
                    ? this->lower.at(next_i)
                    : this->upper.at(next_i);
            }
        }

    // private:
    //     static constexpr auto add(std::uint32_t v1, std::uint32_t v2) { return min_size_fmpint{std::uint64_t(v1) + std::uint64_t(v2)}; }
    //     static constexpr auto mul(std::uint32_t v1, std::uint32_t v2) { return min_size_fmpint{std::uint64_t(v1) * std::uint64_t(v2)}; }
    //     template <class T>
    //     static constexpr auto add(const T& v1, const T& v2)
    //     {
    //         using next_size_fmpint = fmpint<(T::size << 1)>;
    //         const auto l = add(v1.lower, v2.lower);
    //         const auto u = add(v1.upper, v2.upper);
    //         const auto carry = add(l.upper, u.lower);

    //         auto lower = T{l.lower};
    //         lower.upper = carry.lower;
    //         auto r = next_size_fmpint{lower};
    //         r.upper = add(u.upper, carry.upper);
    //         return r;
    //     }
    //     template <class T>
    //     static constexpr auto mul(const T& v1, const T& v2)
    //     {
    //         const auto l = mul(v1,lower, v2.lower);
    //         const auto m = add(
    //             mul(v1.lower, v2.upper),
    //             mul(v1.upper, v2.lower)
    //         );
    //         auto m_l = T{0};
    //         m_l.upper = m.lower.lower;
    //         auto m_u = T{m.lower.upper};
    //         m_u.upper = m.upper.lower;
    //         const auto u = mul(v1.upper, v2.upper);

    //         const auto lower = add(l, m_l);
    //         const auto upper = add(u, m_u);
    //         lower.upper = add(upper.lower, lower.upper).lower;
    //         return lower;
    //     }
    };

    using int128_t = fmpint<(sizeof(std::uint64_t) << 1)>;
    using int256_t = fmpint<(sizeof(std::uint64_t) << 2)>;
    using int512_t = fmpint<(sizeof(std::uint64_t) << 3)>;
}

#endif
