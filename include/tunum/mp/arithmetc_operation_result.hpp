#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MP_ARITHMETIC_OPERATION_RESULT_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MP_ARITHMETIC_OPERATION_RESULT_HPP

#include TUNUM_COMMON_INCLUDE(submodule_loader.hpp)
#include TUNUM_COMMON_INCLUDE(mp/get_integral_size.hpp)
#include TUNUM_COMMON_INCLUDE(mp/get_int.hpp)

namespace tunum::tpfn
{
    // 四則演算子による算術演算の結果型を取得
    // @tparam L 左オペランド型
    // @tparam R 右オペランド型
    template <class L, class R>
    struct arithmetc_operation_result;

    // 組み込みの算術型は、実装定義
    template <class L, class R>
    requires (std::is_arithmetic_v<L> && std::is_arithmetic_v<R>)
    struct arithmetc_operation_result<L, R>
        : public std::type_identity<decltype(std::declval<L>() + std::declval<R>())>
    {};

    // fmpintと組み込みの整数型、またはどちらもfmpintの場合、下記の順序で型を決める
    // 結果は必ずfmpint
    // 1. byteサイズが異なる場合、サイズが大きいほうの型
    // 2. byteサイズが等しく、符号が異なる場合は表現可能な絶対値が大きいほう(符号なし)の型
    template <class L, class R>
    requires (
        tump::count_if_v<::tunum::is_fmpint, tump::list<L, R>> != 0
        && tump::count_if_v<::tunum::is_integral, tump::list<L, R>> == 2
    )
    struct arithmetc_operation_result<L, R>
    {
        static constexpr auto _l_size = get_integral_size_v<L>;
        static constexpr auto _r_size = get_integral_size_v<R>;
        static constexpr auto _l_is_signed = !is_unsigned_v<L>;
        static constexpr auto _r_is_signed = !is_unsigned_v<R>;

        using type = fmpint<
            (std::max)(_l_size, _r_size),
            _l_size == _r_size
                // 二つの型のサイズが等しいとき、片方が符号なしであれば、符号なし整数となる
                ? _l_is_signed && _r_is_signed
                // 二つの型のサイズが異なるとき、サイズが大きいほうの型の符号となる
                : (_l_size > _r_size ? _l_is_signed : _r_is_signed)
        >;
    };

    // fmpintと浮動小数点型は浮動小数点型優先
    template <class L, class R>
    requires (
        tump::count_if_v<::tunum::is_fmpint, tump::list<L, R>> == 1
        && tump::count_if_v<tump::is_floating_point, tump::list<L, R>> == 1
    )
    struct arithmetc_operation_result<L, R>
        : public tump::fn::find_if<tump::is_floating_point, tump::list<L, R>>
    {};
}

namespace tunum
{
    // 四則演算子による算術演算の結果型を取得する
    // @tparam L 左オペランド型
    // @tparam R 右オペランド型
    using arithmetc_operation_result = tump::cbk<tpfn::arithmetc_operation_result, 2>;

    // 四則演算子による算術演算の結果型を取得する
    // @tparam L 左オペランド型
    // @tparam R 右オペランド型
    template <class L, class R>
    using arithmetc_operation_result_t = typename tpfn::arithmetc_operation_result<L, R>::type;
}

#endif
