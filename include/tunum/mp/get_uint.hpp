#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MP_GET_UINT_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MP_GET_UINT_HPP

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
    // 任意サイズの符号なし整数型を取得
    // 2の累乗以外のByteSizeは直上の2累乗値に引き上げられる
    // @tparam ByteSize
    template <std::size_t ByteSize>
    struct get_uint : public get_uint<std::bit_ceil(ByteSize)> {};
    template <> struct get_uint<(1 << 0)> : public std::type_identity<std::uint8_t> {};
    template <> struct get_uint<(1 << 1)> : public std::type_identity<std::uint16_t> {};
    template <> struct get_uint<(1 << 2)> : public std::type_identity<std::uint32_t> {};
    template <> struct get_uint<(1 << 3)> : public std::type_identity<std::uint64_t> {};
    template <std::size_t ByteSize>
    requires ((1 << 3) < ByteSize)
    struct get_uint<ByteSize> : public std::type_identity<
        fmpint<std::bit_ceil(ByteSize), false>
    > {};
}

namespace tunum
{
    // 任意サイズの符号なし整数型を取得
    // 2の累乗以外のByteSizeは直上の2累乗値に引き上げられる
    // @tparam ByteSize
    template <std::size_t ByteSize>
    using get_uint_t = typename tpfn::get_uint<ByteSize>::type;
}

#endif
