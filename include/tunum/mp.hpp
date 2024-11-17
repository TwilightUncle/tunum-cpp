// ---------------------------------------
// メタプログラミング
// ---------------------------------------
#ifndef TUNUM_INCLUDE_GUARD_TUNUM_MP_HPP
#define TUNUM_INCLUDE_GUARD_TUNUM_MP_HPP

#ifndef TUNUM_COMMON_INCLUDE
#define TUNUM_COMMON_INCLUDE(path) <tunum/path>
#endif

#include TUNUM_COMMON_INCLUDE(mp/integral_to_floating.hpp)
#include TUNUM_COMMON_INCLUDE(mp/is_unsigned_fmpint.hpp)
#include TUNUM_COMMON_INCLUDE(mp/get_int.hpp)
#include TUNUM_COMMON_INCLUDE(mp/get_large_integral.hpp)
#include TUNUM_COMMON_INCLUDE(mp/is_constexpr.hpp)
#include TUNUM_COMMON_INCLUDE(mp/math_calculation_result.hpp)
#include TUNUM_COMMON_INCLUDE(mp/is_fe_holder.hpp)

#endif
