#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MP_MATH_CALCULATION_RESULT_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MP_MATH_CALCULATION_RESULT_HPP

#include TUNUM_COMMON_INCLUDE(mp/arithmetc_operation_result.hpp)

namespace tunum::tpfn
{
    // 数学関数における任意数の引数の結果型取得
    // @tparam ...ArgsT 1つ以上の引数
    template <class... ArgsT>
    struct math_calclation_result;

    // 引数が1つだけの場合
    template <class T>
    struct math_calclation_result<T>
        : public std::type_identity<T>
    {};

    // 引数が2つ以上
    template <class ArgT1, class... ArgsT>
    requires (sizeof...(ArgsT) > 0)
    struct math_calclation_result<ArgT1, ArgsT...>
        : public tump::fn::foldl<
            ::tunum::arithmetc_operation_result,
            ArgT1,
            tump::list<ArgsT...>
        >
    {};
}

namespace tunum
{
    // 数学関数における任意数の引数の結果型取得
    // @tparam N 検査する数学関数の引数の数。0以上
    template <std::size_t N = 2>
    requires (N > 0)
    using math_calclation_result = tump::cbk<tpfn::math_calclation_result, N>;

    // 数学関数における任意数の引数の結果型取得
    // @tparam ...ArgsT 1つ以上の引数
    template <class... ArgsT>
    using math_calclation_result_t = typename tpfn::math_calclation_result<ArgsT...>::type;
}

#endif
