#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MP_ARITHMETIC_OPERATION_RESULT_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MP_ARITHMETIC_OPERATION_RESULT_HPP

#include TUNUM_COMMON_INCLUDE(submodule_loader.hpp)

namespace tunum::fn
{
    // 四則演算子による算術演算の結果型を取得
    // @tparam L 左オペランド型
    // @tparam R 右オペランド型
    template <class L, class R>
    requires (std::is_arithmetic_v<L> && std::is_arithmetic_v<R>)
    struct arithmetc_operation_result
        : std::type_identity<decltype(std::declval<L>() + std::declval<R>())> {};
}

namespace tunum
{
    // 四則演算子による算術演算の結果型を取得する
    // @tparam L 左オペランド型
    // @tparam R 右オペランド型
    using arithmetc_operation_result = tump::cbk<fn::arithmetc_operation_result, 2>;

    // 四則演算子による算術演算の結果型を取得する
    // @tparam L 左オペランド型
    // @tparam R 右オペランド型
    template <class L, class R>
    using arithmetc_operation_result_t = typename fn::arithmetc_operation_result<L, R>::type;
}

#endif
