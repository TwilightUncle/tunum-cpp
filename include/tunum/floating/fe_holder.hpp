#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_FE_HOLDER_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_FE_HOLDER_HPP

#include <cfenv>
#include TUNUM_COMMON_INCLUDE(floating/std_info.hpp)

namespace tunum
{
    // 浮動小数点例外を参照可能な算術型
    template <std::floating_point T>
    struct fe_holder
    {
        std::fexcept_t fexcepts = {};
        T value = {};

        // -------------------------------------------
        // コンストラクタ
        // -------------------------------------------

        constexpr fe_holder() = default;

        // 浮動小数点型の値より作成
        constexpr fe_holder(T v, std::fexcept_t e = {}) noexcept
            : value(v)
            , fexcepts(e)
        {}

        // 別のfe_holderオブジェクトより生成
        template <std::floating_point U>
        constexpr fe_holder(const fe_holder<U>& feh) noexcept
            : fexcepts(feh.fexcepts)
            , value(feh.value)
        {}

        // 任意の式を含むラムダ式や、コールバック関数の実行よりオブジェクト生成
        // コールバック関数の実行に伴う浮動小数点例外も記録する
        template <class Lambda, class... Args>
        requires (std::is_invocable_r_v<T, Lambda, Args...>)
        constexpr fe_holder(const Lambda& fn, const Args&... args)
        {
            auto before_e = std::fexcept_t{};
            if (!std::is_constant_evaluated()) {
                std::fegetexceptflag(&before_e, FE_ALL_EXCEPT);
                // 浮動小数点例外をクリアし、受け取ったコールバック内で発生した浮動小数点例外のみ記録、取得
                std::feclearexcept(FE_ALL_EXCEPT);
            }

            value = fn(args...);

            if (!std::is_constant_evaluated()) {
                std::fegetexceptflag(&fexcepts, FE_ALL_EXCEPT);
                // クリア前の例外も含め書き戻す
                std::fesetexceptflag(&(before_e |= fexcepts), FE_ALL_EXCEPT);
            }
            else {
                // コンパイル時のみ結果についての検証も実施
                const auto result_info = floating_std_info{value};
                if (result_info.is_nan())
                    fexcepts |= FE_INEXACT;
                if (result_info.is_infinity())
                    fexcepts |= FE_OVERFLOW;
                if (result_info.is_denormalized())
                    fexcepts |= FE_UNDERFLOW;
                // 結果がゼロの場合も、UNDERFLOWが起こりうるが、
                // ここで得られる情報では、結果のゼロが例外であるかどうか判定できない
            }
        }

        // --------------------------------------
        // 演算子オーバーロード
        // --------------------------------------

        constexpr operator T() const noexcept
        { return value; }

        constexpr explicit operator bool() const noexcept
        { return static_cast<bool>(value); }

        constexpr bool operator!() const noexcept
        { return !value; }

        // -------------------------------------
        // メンバ関数
        // -------------------------------------

        // 保持している浮動小数点例外は空だが、
        // 同じ値を持つオブジェクトを生成
        constexpr auto clear_except() const noexcept
        { return fe_holder{value}; }

        // 引数で指定された例外が発生しているか判定
        // 特に引数を指定しなければ全ての例外について発生を検査する
        constexpr bool has_fexcept(std::fexcept_t test_flags = FE_ALL_EXCEPT) const noexcept
        { return static_cast<bool>(fexcepts & test_flags); }

        // ゼロ除算が発生しているか検査する
        constexpr bool has_divbyzero() const noexcept
        { return has_fexcept(FE_DIVBYZERO); }

        // 不正確な結果であるか検査する
        constexpr bool has_inexact() const noexcept
        { return has_fexcept(FE_INEXACT); }

        // 不正な演算か判定する
        constexpr bool has_invalid() const noexcept
        { return has_fexcept(FE_INVALID); }

        // オーバーフローが発生しているか
        constexpr bool has_overflow() const noexcept
        { return has_fexcept(FE_OVERFLOW); }

        // アンダーフローが発生しているか
        constexpr bool has_underflow() const noexcept
        { return has_fexcept(FE_UNDERFLOW); }

        // template <std::floating_point U>
        // constexpr auto add(fe_holder<U> r) const noexcept
        // {
        //     auto args_except = fexcepts | r.fexcepts;

        //     if (std::is_constant_evaluated()) {
        //         const auto infol = floating_std_info{value};
        //         const auto infor = floating_std_info{x.value};
        //         // 符号が異なる無限同士の加算(減算)は無効な演算
        //         if (infol.is_infinity() && infor.is_infinity())
        //             return  std:: result |= FE_INVALID * (infol.sign() != infor.sign());
        //     }
        //     const auto result = fe_holder([] { return value + r.value; });
        //     result |= fexcepts | r.fexcepts;
        // }
    };

    // コールバック関数コンストラクタ用の推論補助
    template <class Lambda, class... Args> 
    requires (std::invocable<Lambda, Args...>)
    fe_holder(Lambda, Args...)
        -> fe_holder<std::invoke_result_t<Lambda, Args...>>;
}

#endif
