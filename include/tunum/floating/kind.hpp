#ifndef TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_KIND_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_FLOATING_KIND_HPP

namespace tunum
{
    // 浮動小数点型の値の種類
    enum class floating_value_kind
    {
        // ゼロ
        ZERO,
        // 正規化数
        NORM,
        // 非正規化数
        DENORM,
        // 無限大
        INF,
        // 非数
        NAN_
    };
}

#endif
