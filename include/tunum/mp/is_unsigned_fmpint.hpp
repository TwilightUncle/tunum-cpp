#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MP_IS_UNSIGNED_FMPINT_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MP_IS_UNSIGNED_FMPINT_HPP

#include TUNUM_COMMON_INCLUDE(mp/is_fmpint.hpp)

namespace tunum::tpfn
{
    // 符号なし fmpint か判定
    // @tparam T 検査対象型
    template <class T>
    struct is_unsigned_fmpint : public std::false_type {};
    template <std::size_t Bytes>
    struct is_unsigned_fmpint<fmpint<Bytes, false>> : public std::true_type {};

    // 組み込みの符号なし整数型または、符号なしfmpintかどうか判定
    // @tparam T 検査対象型
    template <class T>
    using is_unsigned = std::disjunction<std::is_unsigned<T>, is_unsigned_fmpint<T>>;
}

namespace tunum
{
    // fmpintかどうか判定
    // @tparam T 検査対象型
    using is_unsigned_fmpint = tump::cbk<tpfn::is_unsigned_fmpint, 1>;

    // 組み込みの符号なし整数型または、符号なしfmpintかどうか判定
    // @tparam T 検査対象型
    using is_unsigned = tump::cbk<tpfn::is_unsigned, 1>;

    // fmpintかどうか判定
    // @tparam T 検査対象型
    template <class T>
    constexpr bool is_unsigned_fmpint_v = tpfn::is_unsigned_fmpint<T>::value;

    // 組み込みの符号なし整数型または、符号なしfmpintかどうか判定
    // @tparam T 検査対象型
    template <class T>
    constexpr bool is_unsigned_v = tpfn::is_unsigned<T>::value;
}

#endif
