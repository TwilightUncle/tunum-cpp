#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MP_IS_CONSTEXPR_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MP_IS_CONSTEXPR_HPP

namespace tunum
{
    /// ある式がコンパイル時評価可能か判定。
    /// @code
    /// if constexpr (is_constexpr([]{ /* any expression */ }))
    ///     ...
    /// @endcode
    /// @param Lambda ラムダ式の内部にて判定対象の式を記述
    /// @note 参考: https://yohhoy.hatenadiary.jp/entry/20230908/p1
    /// TODO: 汎用的なため、tumpへ定義を移動か？
    // 特定コンパイラで判定がうまくいかない?ため、いったん使用禁止
    // template <class Lambda, int = (Lambda{}(), 0)>
    // constexpr bool is_constexpr(Lambda) { return true; }
    // constexpr bool is_constexpr(...) { return false; }

    // 可能な限りFn1を優先的に実行する
    // コンパイル時にFn1が評価できない場合、代替としてfn2を実行する
    // 判定がうまくいかない?ため、いったん使用禁止
    // template <class Fn1, class Fn2>
    // struct invoke_constexpr
    // {
    //     static constexpr auto fn1 = Fn1{};
    //     static constexpr auto fn2 = Fn2{};

    //     constexpr invoke_constexpr(Fn1, Fn2) noexcept {}

    //     template <class... Args>
    //     requires (!std::is_constant_evaluated() || is_constexpr([] { return fn1(Args{}...); }))
    //     constexpr auto operator()(const Args&... args) const
    //     { return fn1(args...); }

    //     template <class... Args>
    //     constexpr auto operator()(const Args&... args) const
    //     {
    //         static_assert(is_constexpr([] { return fn2(Args{}...); }));
    //         return fn2(args...);
    //     }
    // };

    // コンセプトを経由したコンパイル時評価可能であることの制約
    // @tparam ... 任意の定数式
    // @note 参考: https://yohhoy.hatenadiary.jp/entry/20190528/p1
    template <auto> struct require_constant;
}

#endif
