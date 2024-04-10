#include <gtest/gtest.h>
#include <tunum/floating.hpp>

TEST(TunumFloatingTest, StdInfoTest)
{
    constexpr auto info1 = tunum::floating_std_info{0.};
    EXPECT_TRUE(info1.is_zero());
}
