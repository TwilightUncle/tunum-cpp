#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MP_GET_INT_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MP_GET_INT_HPP

#include <bit>
#include TUNUM_COMMON_INCLUDE(submodule_loader.hpp)

namespace tunum
{
    // 前方宣言
    template <std::size_t Bytes, bool Signed>
    struct fmpint;
}

namespace tunum::tpfn
{
    // 任意サイズの符号あり整数型を取得
    // 2の累乗以外のByteSizeは直上の2累乗値に引き上げられる
    // @tparam ByteSize
    template <std::size_t ByteSize>
    struct get_int : public get_int<std::bit_ceil(ByteSize)> {};
    template <> struct get_int<(1 << 0)> : public std::type_identity<std::int8_t> {};
    template <> struct get_int<(1 << 1)> : public std::type_identity<std::int16_t> {};
    template <> struct get_int<(1 << 2)> : public std::type_identity<std::int32_t> {};
    template <> struct get_int<(1 << 3)> : public std::type_identity<std::int64_t> {};
    template <std::size_t ByteSize>
    requires ((1 << 3) < ByteSize)
    struct get_int<ByteSize> : public std::type_identity<
        fmpint<std::bit_ceil(ByteSize), true>
    > {};
}

namespace tunum
{
    // 任意サイズの符号あり整数型を取得
    // 2の累乗以外のByteSizeは直上の2累乗値に引き上げられる
    // @tparam ByteSize
    template <std::size_t ByteSize>
    using get_int_t = typename tpfn::get_int<ByteSize>::type;
}

#endif
