#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MP_GET_INTEGRAL_SIZE_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MP_GET_INTEGRAL_SIZE_HPP

#include TUNUM_COMMON_INCLUDE(mp/is_fmpint.hpp)

namespace tunum::tpfn
{
    // 組み込みの整数型または、fmpintのbyteサイズを取得する
    // @tparam T 検査対象型
    template <class T>
    struct get_integral_size : public std::integral_constant<int, -1> {};
    template <std::integral T>
    struct get_integral_size<T> : public std::integral_constant<int, sizeof(T)> {};
    template <std::size_t Bytes, bool Signed>
    struct get_integral_size<fmpint<Bytes, Signed>> : public std::integral_constant<int, fmpint<Bytes, Signed>::size> {};
}

namespace tunum
{
    // fmpintかどうか判定
    // @tparam T 検査対象型
    using get_integral_size = tump::cbk<tpfn::get_integral_size, 1>;

    // fmpintかどうか判定
    // @tparam T 検査対象型
    template <class T>
    constexpr int get_integral_size_v = tpfn::get_integral_size<T>::value;
}

#endif
