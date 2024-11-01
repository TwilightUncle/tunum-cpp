#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_IMPL_ARITHMETIC_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FMPINT_IMPL_ARITHMETIC_HPP

#include TUNUM_COMMON_INCLUDE(fmpint/impl/bit_operator.hpp)

namespace tunum::_fmpint_impl
{
    // ----------------------------------
    // クラス実装のうち、算術演算の実装を別クラスとして分離
    // ----------------------------------

    // fmpintの加算処理の実装
    template <std::size_t Bytes, bool Signed>
    struct arithmetic
    {
        using fi = fmpint<Bytes, Signed>;
        static constexpr auto size = fi::size;
        using half_fi = typename fi::half_fmpint;
        using double_fi = fmpint<(size << 1), Signed>;
        static constexpr auto is_min_size = fi::is_min_size;
        static constexpr auto max_digits2 = fi::max_digits2;

        // 左右オペランド
        fi op_l;
        fi op_r;

        // 判定用
        bool is_zero_op_l_l;
        bool is_zero_op_l_u;
        bool is_zero_op_r_l;
        bool is_zero_op_r_u;

        constexpr arithmetic(const fi& op_l, const fi& op_r) noexcept
            : op_l(op_l)
            , op_r(op_r)
            , is_zero_op_l_l(!op_l.lower)
            , is_zero_op_l_u(!op_l.upper)
            , is_zero_op_r_l(!op_r.lower)
            , is_zero_op_r_u(!op_r.upper)
        {}

        // ----------------------------
        // 補助関数軍
        // ----------------------------

        // 左右オペランドについて、lowerとupperのどちらか片方が必ず0
        constexpr bool is_not_duplicated()
        {
            return is_zero_op_l_l && is_zero_op_r_u
                || is_zero_op_l_u && is_zero_op_r_l;
        }

        // どちらかあるいは、両方ゼロ
        constexpr bool is_either_zero()
        {
            return is_zero_op_l_l && is_zero_op_l_u
                || is_zero_op_r_l && is_zero_op_r_u;
        }

        // ----------------------------
        // 加算
        // ----------------------------

        // 加算
        // @param is_calculated_inv 桁上り判定用のビット反転が計算済みかどうか
        constexpr fi add(bool is_calculated_inv = false, const fi& inv_op_l = fi{}) noexcept
        {
            // 確実にbitの重複がない場合、論理和を取ることで加算となる
            if (is_not_duplicated() || is_either_zero())
                return op_l | op_r;

            // ビット反転の計算がない場合は、ここで実施。
            // より小さい型へ、再帰的に同様の計算が発生してしまうため、
            // is_calculated_invを参照して、一度のみ計算させる。
            const auto inv_op_l_lower = is_calculated_inv ? inv_op_l.lower : ~op_l.lower;

            // 2 桁の筆算のような感じ
            auto result = fi{};
            result.lower = add_minor(op_l.lower, op_r.lower, is_calculated_inv, inv_op_l_lower);
            result.upper = add_minor(op_l.upper, op_r.upper, is_calculated_inv, inv_op_l.upper);

            // 桁上りがある場合
            if (inv_op_l_lower < op_r.lower)
                result.upper = add_minor(result.upper, 1, false);
            return result;
        }

        // 再帰の制御
        static constexpr half_fi add_minor(const half_fi& l, const half_fi& r, bool is_calculated_inv, const fi& inv_op_l = fi{}) noexcept {
            if constexpr (is_min_size)
                return l + r;
            else
                return arithmetic<(size >> 1), Signed>{l, r}.add(is_calculated_inv, inv_op_l);
        }

        // ----------------------------
        // 乗算
        // ----------------------------

        // 乗算
        // TODO: FFTによる高速化
        constexpr double_fi mul() noexcept { return mul_karatsuba(); }

        // カラツバ法による乗算の実装
        constexpr double_fi mul_karatsuba() noexcept
        {
            if (is_either_zero())
                return double_fi{0};

            // カラツバ法
            const auto r1 = double_fi{
                (!is_zero_op_l_u && !is_zero_op_r_u) ? fi{mul_minor(op_l.upper, op_r.upper)} : fi{},
                (!is_zero_op_l_l && !is_zero_op_r_l) ? fi{mul_minor(op_l.lower, op_r.lower)} : fi{}
            };

            // たすき掛けのクロスしてる部分
            const auto middle_1 = fi{op_l.lower} + op_l.upper;
            const auto middle_2 = fi{op_r.lower} + op_r.upper;
            const auto is_zero_mid_1u = !middle_1.upper;
            const auto is_zero_mid_2u = !middle_2.upper;
            // N / 2 + 1 桁となる場合も考慮
            const auto r2 = double_fi{
                    fi{!is_zero_mid_1u && !is_zero_mid_2u ? 1 : 0},
                    fi{mul_minor(middle_1.lower, middle_2.lower)}
                }
                + double_fi{is_zero_mid_1u ? fi{} : fi{fi{middle_2.lower}, fi{}}}
                + double_fi{is_zero_mid_2u ? fi{} : fi{fi{middle_1.lower}, fi{}}}
                - double_fi{r1.upper}
                - double_fi{r1.lower};

            return r1 + (r2 << (size * 8 / 2));
        }

        // 再帰の制御
        static constexpr fi mul_minor(const half_fi& l, const half_fi& r) noexcept
        {
            if constexpr (is_min_size)
                return fi{std::uint64_t(l) * std::uint64_t(r)};
            else
                return arithmetic<(size >> 1), Signed>{l, r}.mul();
        }

        // ----------------------------
        // 除算
        // ----------------------------

        // 除算
        // v1 と v2の差が大きく、両端の連続した0が少ない値ほど計算量も増える
        constexpr fi div()
        {
            using minor_arith = arithmetic<(size >> 1), Signed>;
            // 重いので計算せずとも自明なものはここではじいておく
            if (!op_r)
                throw std::invalid_argument{"0 div."};
            if (!op_l || op_l < op_r)
                return fi{};
            if (op_r == 1)
                return op_l;

            if constexpr (is_min_size)
                // 組み込みの演算子使えるならそっち優先
                return fi{std::uint64_t{op_l} / std::uint64_t{op_r}};
            else {
                const auto opr_l_bit_op = bit_operator{op_l};
                const auto opr_r_bit_op = bit_operator{op_r};
                // 両側の0ビットを除去したビット幅がより小さい型でも計算可能な際はそちらへ処理を委譲
                if (
                    const auto min_zero_r_cnt = (std::min)(opr_l_bit_op.countr_zero_bit(), opr_r_bit_op.countr_zero_bit());
                    max_digits2 - (opr_l_bit_op.countl_zero_bit() + min_zero_r_cnt) <= max_digits2 / 2
                ) {
                    const auto _quo = minor_arith{
                        (op_l >> min_zero_r_cnt).lower,
                        (op_r >> min_zero_r_cnt).lower
                    }.div();
                    return fi{_quo};
                }

                // TODO: v1とv2のbit幅が近い場合は、多分筆算アルゴリズムのほうが多分早いので、そのあたりの最適な分岐点を考える
                // bit幅の差20は仮
                return (opr_l_bit_op.get_bit_width() - opr_r_bit_op.get_bit_width() < 20)
                    ? div_bit_column()
                    : div_newton();
            }
        }

        // 2進数による、筆算のような除算実装
        constexpr fi div_bit_column() noexcept
        {
            // v1 と v2 の２進数桁数の差より、v2のシフト数を取得
            const std::size_t lshift_cnt = bit_operator{op_l}.get_bit_width() - bit_operator{op_r}.get_bit_width();
            auto rem = fi{op_l};
            auto quo = fi{};
            for (std::size_t i = 0; i <= lshift_cnt; i++) {
                const auto shifted = op_r << (lshift_cnt - i);
                if (rem >= shifted) {
                    quo.set_bit(lshift_cnt - i, true);
                    rem -= shifted;
                }
            }
            return quo;
        }

        // ニュートン法を用いた逆数近似による除算の実装
        // 参考: http://www.hundredsoft.jp/win7blog/log/eid94.html
        // 
        // v1とv2のbit幅の合計が、double_fmpintの最大bit幅 - 1以上のとき、
        // 一部計算時の値のオーバーフローによって正しく計算できないため、
        // 呼び出し側でbit幅確認の上、オーバーフローが予測される際は_div_bit_columnを呼び出すようにする。
        // ※仮にオーバーフローするような場合を実装したとしても、_div_bit_columnのほうが速い
        // TODO: いい感じの初期値を決定する
        constexpr fi div_newton() noexcept
        {
            using major_arith = arithmetic<(size << 1), Signed>;
            const int l_bit_width = bit_operator{op_l}.get_bit_width();
            const int r_bit_width = bit_operator{op_r}.get_bit_width();
            const int n = l_bit_width + r_bit_width;
            const auto x = calc_reciprocal_by_newton(op_r, n, double_fi{1} << l_bit_width);

            // 分母の逆数の近似値と分子を乗算(桁上り考慮のため、_mulを使用)
            const auto detect_result = major_arith{double_fi{op_l}, x}.mul() >> n;
            const auto detect_inc = detect_result + 1;
            // 1の誤差有無判定がてら結果返却
            return op_l >= (detect_inc * op_r)
                ? detect_inc
                : detect_result;
        }

        // ニュートン法による逆数の近似値xを算出
        static constexpr auto calc_reciprocal_by_newton(const fi& q, const int n, const double_fi& init) noexcept
        {
            using major_arith = arithmetic<(size << 1), Signed>;
            auto x = init;
            const auto c2 = double_fi{2} << n;
            for (auto m = double_fi{}; m != x;) {
                m = x;
                // 桁上り考慮のため_mulを使用
                x = major_arith{x, c2 - q * x}.mul() >> n;
            }
            return x;
        }
    };
}

#endif
