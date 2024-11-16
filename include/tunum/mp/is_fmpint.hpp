#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MP_IS_FMPINT_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MP_IS_FMPINT_HPP

#include TUNUM_COMMON_INCLUDE(submodule_loader.hpp)

namespace tunum
{
    // 前方宣言
    template <std::size_t Bytes, bool Signed>
    struct fmpint;
}

namespace tunum::tpfn
{
    // fmpintかどうか判定
    // @tparam T 検査対象型
    template <class T>
    struct is_fmpint : public std::false_type {};
    template <std::size_t Bytes, bool Signed>
    struct is_fmpint<fmpint<Bytes, Signed>> : public std::true_type {};
}

namespace tunum
{
    // fmpintかどうか判定
    // @tparam T 検査対象型
    using is_fmpint = tump::cbk<tpfn::is_fmpint, 1>;

    // fmpintかどうか判定
    // @tparam T 検査対象型
    template <class T>
    constexpr bool is_fmpint_v = tpfn::is_fmpint<T>::value;
}

#endif
