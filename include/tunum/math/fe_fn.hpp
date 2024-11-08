#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MATH_FE_FN_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MATH_FE_FN_HPP

#include <cfenv>
#include TUNUM_COMMON_INCLUDE(submodule_loader.hpp)
#include TUNUM_COMMON_INCLUDE(floating.hpp)
#include TUNUM_COMMON_INCLUDE(math/fe_holder.hpp)

namespace tunum
{
    // 浮動小数点例外を考慮した算術関数・演算子の基底
    // 引数の検証、定義した算術関数の実行、結果の検証を行い、必要に応じて浮動小数点例外を設定する
    // fe_holderに包まれた浮動小数点型しか扱わない
    // fe_holderを継承することにより、ダウンキャストで結果型とすることができるため、コンストラクタ実行のみで関数の機能を表現
    template <std::floating_point... ArgsT>
    requires (sizeof...(ArgsT) > 0)
    struct fe_fn
        : public fe_holder<
            tump::mp_max_t<tump::list<ArgsT...>>
        >
    {
        // 最大の範囲を持つ型を抽出
        // 関数内での計算は全て、calc_tにキャストの上計算を行う
        using calc_t = tump::mp_max_t<tump::list<ArgsT...>>;
        using use_fe_holder_t = fe_holder<calc_t>;

        static constexpr auto validate_arg_default = [](
            tump::left_t<floating_std_info<calc_t>&&, ArgsT>...
        ) {
            return std::tuple{ std::fexcept_t{}, true, calc_t{} };
        };

        static constexpr auto check_after_default = [](
            floating_std_info<calc_t>&&,
            tump::left_t<floating_std_info<calc_t>&&, ArgsT>...
        ) {
            return std::fexcept_t{};
        };

        // このコンセプトきしょいな
        // だが、GCC11でvirtual constexprのオバーライドうまくいかなかったのでコンストラクタで渡すしかない
        // 多態性メンバ関数の判定周りの理解が浅いか？
        template <
            TuInvocableR<
                calc_t,
                tump::left_t<calc_t, ArgsT>...
            > RunFn,
            TuInvocableR<
                std::tuple<std::fexcept_t, bool, calc_t>,
                tump::left_t<floating_std_info<calc_t>&&, ArgsT>...
            > ValidateFn,
            TuInvocableR<
                std::fexcept_t,
                floating_std_info<calc_t>&&, tump::left_t<floating_std_info<calc_t>&&, ArgsT>...
            > CheckAfterFn
        >
        constexpr fe_fn(
            const RunFn& run,
            const ValidateFn& validate_args,
            const CheckAfterFn& check_after, 
            const fe_holder<ArgsT>&... args
        )
            : use_fe_holder_t()
        {
            // 事前検証と浮動小数点例外の伝播
            const auto [e, is_run, result_value] = validate_args(floating_std_info{calc_t{args.value}}...);
            this->fexcepts |= e | (... | args.fexcepts);

            // 演算未実施の場合
            if (!is_run) {
                this->value = result_value;
                return;
            } 

            // メインの機能実行
            const auto calc_result = use_fe_holder_t([&] { return run(calc_t{args.value}...); } );
            this->value = calc_result.value;

            // runにより発生した例外と、事後チェックの例外をマージ
            this->fexcepts |= calc_result.fexcepts
                | check_after(
                    floating_std_info{this->value},
                    floating_std_info{calc_t{args.value}}...
                );
        }

        template <
            TuInvocableR<
                calc_t,
                tump::left_t<calc_t, ArgsT>...
            > RunFn,
            TuInvocableR<
                std::tuple<std::fexcept_t, bool, calc_t>,
                tump::left_t<floating_std_info<calc_t>&&, ArgsT>...
            > ValidateFn
        >
        constexpr fe_fn(
            const RunFn& run,
            const ValidateFn& validate_arg,
            const fe_holder<ArgsT>&... args
        )
            : fe_fn(run, validate_arg, check_after_default, args...)
        {}

        template <
            TuInvocableR<
                calc_t,
                tump::left_t<calc_t, ArgsT>...
            > RunFn
        >
        constexpr fe_fn(
            const RunFn& run,
            const fe_holder<ArgsT>&... args
        )
            : fe_fn(run, validate_arg_default, args...)
        {}

        // // 引数の検証を行う
        // // 実装は任意
        // // 本メソッドには、std_floating_infoが適用された値が渡されるので、
        // // 定義域など確認する処理を継承先で実装すること
        // // 戻り値は新規で発生する浮動小数点例外(引数から予測可能なもののみ)と演算の実施有無および、演算を未実施の場合の結果
        // virtual constexpr std::tuple<std::fexcept_t, bool, calc_t> validate_args(tump::left_t<floating_std_info<calc_t>&&, ArgsT>...) const
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
        // virtual constexpr std::fexcept_t check_after(floating_std_info<calc_t>&&, tump::left_t<floating_std_info<calc_t>&&, ArgsT>...)
        // { return std::fexcept_t{}; }
    };
}

#endif
