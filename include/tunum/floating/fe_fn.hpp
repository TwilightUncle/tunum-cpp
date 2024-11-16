#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_FE_FN_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_FE_FN_HPP

#include <cfenv>
#include <array>
#include <algorithm>
#include TUNUM_COMMON_INCLUDE(submodule_loader.hpp)
#include TUNUM_COMMON_INCLUDE(floating/fe_holder.hpp)

namespace tunum
{
    // 浮動小数点例外を考慮した算術関数・演算子の基底
    // 引数の検証、定義した算術関数の実行、結果の検証を行い、必要に応じて浮動小数点例外を設定する
    // fe_holderに包まれた浮動小数点型しか扱わない
    // fe_holderを継承することにより、ダウンキャストで結果型とすることができるため、コンストラクタ実行のみで関数の機能を表現
    template <std::fexcept_t RaiseFeFlags, std::floating_point... ArgsT>
    requires (sizeof...(ArgsT) > 0)
    struct fe_fn : public fe_holder<
        tump::mp_max_t<tump::list<ArgsT...>>,
        RaiseFeFlags
    > {
        // 最大の範囲を持つ型を抽出
        // 関数内での計算は全て、calc_tにキャストの上計算を行う
        using calc_t = tump::mp_max_t<tump::list<ArgsT...>>;
        using limits_t = std::numeric_limits<calc_t>;
        using fe_holder_t = fe_holder<calc_t, RaiseFeFlags>;
        using info_t = floating_std_info<calc_t>;
        using validate_result_t = std::tuple<std::fexcept_t, bool, calc_t>;

        // パラメータパック展開用calc_t
        template <class>
        using to_calc_t = calc_t;

        // パラメータパック展開用info_t
        template <class>
        using to_info_ref_t = const info_t&;

        // -------------------------------------------
        // コンストラクタ
        // -------------------------------------------

        // このコンセプトきしょいな
        // だが、GCC11でvirtual constexprのオバーライドうまくいかなかったのでコンストラクタで渡すしかない
        // 多態性メンバ関数の判定周りの理解が浅いか？
        template <
            TuInvocableR<calc_t, to_calc_t<ArgsT>...> RunFn,
            TuInvocableR<validate_result_t, to_info_ref_t<ArgsT>...> ValidateFn,
            TuInvocableR<std::fexcept_t, const info_t&, to_info_ref_t<ArgsT>...> CheckAfterFn
        >
        constexpr fe_fn(
            const RunFn& run,
            const ValidateFn& validate_args,
            const CheckAfterFn& check_after, 
            const fe_holder<ArgsT, RaiseFeFlags>&... args
        )
            : fe_holder_t()
        {
            // 事前検証と浮動小数点例外の伝播
            const auto [e, is_run, result_value] = validate_args(info_t{calc_t{args.value}}...);
            this->fexcepts |= e | (... | args.fexcepts);

            // 演算未実施の場合
            if (!is_run) {
                this->value = result_value;
                return;
            } 

            // メインの機能実行
            const auto calc_result = fe_holder_t([&] { return run(calc_t{args.value}...); } );
            this->value = calc_result.value;

            // runにより発生した例外と、事後チェックの例外をマージ
            this->fexcepts |= calc_result.fexcepts
                | check_after(info_t{this->value}, info_t{calc_t{args.value}}...);
        }

        template <class RunFn, class ValidateFn>
        constexpr fe_fn(
            const RunFn& run,
            const ValidateFn& validate_arg,
            const fe_holder<ArgsT, RaiseFeFlags>&... args
        )
            : fe_fn(run, validate_arg, check_after_default, args...)
        {}

        template <class RunFn>
        constexpr fe_fn(
            const RunFn& run,
            const fe_holder<ArgsT, RaiseFeFlags>&... args
        )
            : fe_fn(run, validate_arg_default, args...)
        {}

        // ---------------------------------------
        // デフォルトの引数・結果検証処理
        // ---------------------------------------

        // コンストラクタのデフォルト引数
        // static constexpr auto validate_arg_default = [](to_info_ref_t<ArgsT>... infos) {
        static constexpr auto validate_arg_default(to_info_ref_t<ArgsT>... infos)
        {
            // 多分どの関数でも引数にNaNが含まれていたら結果はNaN
            // また、この関数の計算にて発生したNaNではないため、ここでの例外は特になし
            return exsists_nan(infos...)
                ? validate_result_t{std::fexcept_t{}, false, info_t::get_nan()}
                // 無限を含む計算は関数によって、結果がNaNや0になることもあるので、継承先で実装を行う
                : validate_result_t{ std::fexcept_t{}, true, calc_t{} };
        }

        // コンストラクタのデフォルト引数
        static constexpr auto check_after_default(const info_t& result_info, to_info_ref_t<ArgsT>...)
        {
            return result_info.is_denormalized()
                ? std::fexcept_t{FE_INEXACT | FE_UNDERFLOW}
                : std::fexcept_t{};
        }

        // // 引数の検証を行う
        // // 実装は任意
        // // 本メソッドには、std_floating_infoが適用された値が渡されるので、
        // // 定義域など確認する処理を継承先で実装すること
        // // 戻り値は新規で発生する浮動小数点例外(引数から予測可能なもののみ)と演算の実施有無および、演算を未実施の場合の結果
        // virtual constexpr validate_result_t validate_args(tump::left_t<info_t&&, ArgsT>...) const
        // { return { std::fexcept_t{}, true, calc_t{} }; };

        // // 関数の実装
        // // virtual constexpr calc_t run(tump::left_t<calc_t, ArgsT>...) const = 0;
        // virtual constexpr calc_t run(tump::left_t<calc_t, ArgsT>...) const {
        //     // gccで = 0とすると、定義されるpure_virtualにconstexprがついていないことでコンパイルエラーとなるみたいなので、例外投げとく
        //     // 呼び出されないはずなので、エラーは起きえないはずだが
        // 　　// →結局こいつが呼び出される
        //     throw std::logic_error("Not implemented");
        //     return calc_t{};
        // };

        // // 結果の解析。実装は任意
        // // 最初の引数に計算の結果が、残りの引数に演算のオペランドを受け取る
        // // 減算や除算の結果、アンダーフローによりゼロとなるような場合の検証を行う
        // virtual constexpr std::fexcept_t check_after(info_t&&, tump::left_t<info_t&&, ArgsT>...)
        // { return std::fexcept_t{}; }

        // -----------------------------------------------
        // 検証用補助関数
        // -----------------------------------------------

        // infosの中に、NaNが含まれる
        template <class... InfosT> 
        static constexpr bool exsists_nan(const InfosT&... infos) noexcept
        { return (... || infos.is_nan()); }

        // infosの中に含まれる無限の数をカウント
        template <class... InfosT>
        static constexpr int count_infinity(const InfosT&... infos) noexcept
        { return (... + int(infos.is_infinity())); }

        // infosの中に含まれる無限のうち、先頭の値を取得
        // ※infoを外したcalc_tの値
        // なければ0
        template <class... InfosT>
        static constexpr calc_t get_first_infinity(const InfosT&... infos) noexcept
        {
            for (const auto& info : std::array{infos...})
                if (info.is_infinity())
                    return calc_t{info};
            return calc_t{};
        }
    };
}

#endif
