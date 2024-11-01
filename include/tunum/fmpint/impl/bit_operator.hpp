#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_IMPL_BIT_OPERATOR_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_IMPL_BIT_OPERATOR_HPP

#include TUNUM_COMMON_INCLUDE(mp.hpp)

namespace tunum::_fmpint_impl
{
    // ----------------------------------
    // クラス実装のうち、ビット操作の実装を別クラスとして分離
    // ----------------------------------

    // fmpintのbit操作の実装
    template <std::size_t Bytes, bool Signed>
    struct bit_operator
    {
        using fi = fmpint<Bytes, Signed>;
        using half_fi = typename fi::half_fmpint;
        using base_data_t = typename fi::base_data_t;
        static constexpr auto size = fi::size;
        static constexpr std::size_t base_data_digits2 = fi::base_data_digits2;
        static constexpr std::size_t data_length = fi::data_length;
        static constexpr std::size_t max_digits2 = fi::max_digits2;
        static constexpr bool is_min_size = fi::is_min_size;

        fi opr;

        constexpr bit_operator(const fi& opr) noexcept
            : opr(opr)
        {}

        // ビット左シフト
        constexpr fi shift_l(std::size_t n) const noexcept
        {
            const auto shift_mod = n % base_data_digits2;
            const auto shift_mul = n / base_data_digits2;

            if (data_length <= shift_mul)
                return fi{};
            
            auto result = rotate_l(n);

            // ローテート演算で回ってきた部分を除去
            const auto fill_mask = ~base_data_t{} << shift_mod;
            for (std::size_t i = 0; i < shift_mul; i++)
                result[i] = 0;
            result[shift_mul] &= fill_mask;
            return result;
        }

        // ビット右シフト
        // 算術シフトか、論理シフトかは処理系に準拠
        // @param n シフト数
        constexpr fi shift_r(std::size_t n) const noexcept
        {
            // 組み込み整数の右シフトが算術シフトとして実装されているか判定
            // ※最上ビットが1の状態で、右シフト後の最上位ビットが1であれば算術シフト、そうでなければ論理シフト
            constexpr auto is_sal = Signed
                ? ((-1) >> 1) == -1
                : ((~0u) >> 1) == (~0u);
            return shift_r(n, is_sal);
        }

        // ビット右シフト
        // @param n シフト数
        // @param is_sal 算術シフト利用の場合true, 論理シフト利用の場合false 
        constexpr fi shift_r(std::size_t n, bool is_sal) const noexcept
        {
            const auto shift_mod = n % base_data_digits2;
            const auto shift_com = (base_data_digits2 - shift_mod) % base_data_digits2;
            const auto shift_mul = n / base_data_digits2;

            if (data_length <= shift_mul)
                return fi{};
            
            auto result = rotate_r(n);

            // ローテート演算で回ってきた部分を除去
            // 算術シフトかつ、最上位ビットが立っている場合、回ってきた部分は1で埋める
            const auto highest_bit = get_back_bit();
            const auto fill_mask = ~base_data_t{} << shift_com;
            const auto is_fill_one = is_sal && highest_bit;
            for (std::size_t i = 0; i < shift_mul; i++)
                result[data_length - 1 - i] = base_data_t{is_fill_one} * ~base_data_t{};
            if (fill_mask != ~base_data_t{}) {
                if (is_fill_one)
                    result[data_length - 1 - shift_mul] ^= fill_mask;
                else
                    result[data_length - 1 - shift_mul] &= ~fill_mask;
            }
            return result;
        }

        // ビット左ローテーション
        constexpr fi rotate_l(int s) const noexcept
        {
            if (!s) return opr;
            if (s < 0) return rotate_r(-s);

            const auto shift_mod = s % base_data_digits2;
            const auto i_diff = s / base_data_digits2;
            const auto shift_com = (base_data_digits2 - shift_mod) % base_data_digits2;

            auto result = fi{opr};
            for (std::size_t i = 0; i < data_length; i++) {
                auto& elem = result[(i + i_diff) % data_length];
                elem = (opr[i] << shift_mod);
                if (shift_com)
                    elem |= (opr[(i + data_length - 1) % data_length] >> shift_com);
            }
            return result;
        }

        // ビット右ローテーション
        constexpr fi rotate_r(int s) const noexcept
        {
            const int l_s = s < 0
                ? -s
                : max_digits2 - (s % max_digits2);
            return rotate_l(l_s);
        }

        // 左側に連続している 0 ビットの数を返却
        constexpr auto countl_zero_bit() const noexcept { return count_continuous_bit(false, true); }

        // 左側に連続している 1 ビットの数を返却
        constexpr auto countl_one_bit() const noexcept { return count_continuous_bit(true, true); }

        // 右側に連続している 0 ビットの数を返却
        constexpr auto countr_zero_bit() const noexcept { return count_continuous_bit(false, false); }

        // 右側に連続している 1 ビットの数を返却
        constexpr auto countr_one_bit() const noexcept { return count_continuous_bit(true, false); }

        // 寝ているビットをカウント
        constexpr auto count_zero_bit() const noexcept { return bit_operator{~opr}.count_one_bit(); }

        // 立っているビットをカウント
        constexpr auto count_one_bit() const noexcept
        { return count_one_bit_minor(opr.lower) + count_one_bit_minor(opr.upper); }

        // より小さい型の再帰制御
        static constexpr auto count_one_bit_minor(const half_fi& v) noexcept
        {
            if constexpr (is_min_size)
                return std::popcount(v);
            else
                return bit_operator<(size >> 1), Signed>{v}.count_one_bit();
        }

        // 全てのビットが立っているかどうか判定。
        // 引数にfalseを指定すると、すべてのビットが寝ているかどうか判定
        constexpr bool is_full_bit(const bool bit = true) const noexcept
        {
            return bit
                ? count_zero_bit() == 0
                : count_one_bit() == 0;
        }

        // 指定されたビットが指定の方向(左右)から連続でいくつ並んでいるかかカウント
        constexpr auto count_continuous_bit(bool bit, bool is_begin_l) const noexcept
        {
            const auto first_bit_cnt = count_continuous_bit_minor(is_begin_l ? opr.upper : opr.lower, bit, is_begin_l); 
            // フルビットじゃなければ連続していないのでその場で返却
            return first_bit_cnt != (base_data_digits2 * data_length / 2)
                ? first_bit_cnt
                : first_bit_cnt + count_continuous_bit_minor(is_begin_l ? opr.lower : opr.upper, bit, is_begin_l);
        }

        // 再帰制御
        static constexpr auto count_continuous_bit_minor(const half_fi& v, bool _bit, bool _is_begin_l) noexcept
        {
            if constexpr (is_min_size) {
                // ビット反転すると結果は同じでしょう
                const auto _v = !_bit ? ~v : v;
                return _is_begin_l ? std::countl_one(_v) : std::countr_one(_v);
            }
            else
                return bit_operator<(size >> 1), Signed>{v}.count_continuous_bit(_bit, _is_begin_l);
        }

        // 格納値を表現するのに必要なビット幅を返却
        constexpr auto get_bit_width() const noexcept { return max_digits2 - countl_zero_bit(); }

        // 指定位置のビットを取得
        constexpr bool get_bit(std::size_t i) const noexcept
        {
            const auto base_data_index = i / base_data_digits2;
            const auto bit_pos = i % base_data_digits2;
            return bool(opr.at(base_data_index) & (base_data_t{1} << bit_pos));
        }

        // 先頭ビットを取得
        constexpr bool get_front_bit() const noexcept { return get_bit(0); }

        // 最後尾ビットを取得
        constexpr bool get_back_bit() const noexcept { return get_bit(max_digits2 - 1); }

        // 指定位置のビットを変更
        constexpr auto change_bit(std::size_t i, bool value) const noexcept
        {
            const auto base_data_index = i / base_data_digits2;
            const auto bit_pos = i % base_data_digits2;
            const auto single_bit = base_data_t{1} << bit_pos;
            
            auto result = fi{opr};
            if (!value)
                result.at(base_data_index) &= (~single_bit);
            else
                result.at(base_data_index) |= single_bit;
            return result;
        }
    };
}

#endif
