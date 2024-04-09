#include <gtest/gtest.h>
#include <tunum/bit.hpp>

TEST(TunumBitTest, BitTest)
{
    constexpr auto case1 = tunum::has_single_bit(1u);
    constexpr auto case2 = tunum::has_single_bit(16u);
    constexpr auto case3 = tunum::has_single_bit(5u);
    EXPECT_TRUE(case1);
    EXPECT_TRUE(case2);
    EXPECT_FALSE(case3);

    constexpr auto case4 = tunum::bit_width(0u);
    constexpr auto case5 = tunum::bit_width(128u);
    constexpr auto case6 = tunum::bit_width(127u);
    EXPECT_EQ(case4, 0u);
    EXPECT_EQ(case5, 8u);
    EXPECT_EQ(case6, 7u);

    constexpr auto case7 = tunum::bit_ceil(0u);
    constexpr auto case8 = tunum::bit_ceil(128u);
    constexpr auto case9 = tunum::bit_ceil(127u);
    EXPECT_EQ(case7, 1u);
    EXPECT_EQ(case8, 128u);
    EXPECT_EQ(case9, 128u);

    constexpr auto case10 = tunum::bit_floor(0u);
    constexpr auto case11 = tunum::bit_floor(128u);
    constexpr auto case12 = tunum::bit_floor(129u);
    EXPECT_EQ(case10, 0u);
    EXPECT_EQ(case11, 128u);
    EXPECT_EQ(case12, 128u);
}

#include <tunum/fmpint.hpp>
using namespace tunum::literals;

TEST(TunumBitTest, FmpintBitTest)
{
    constexpr auto case1 = tunum::has_single_bit(0b0000'0000'1001'0000'0000'0000'0000'0000'0000'0000_ufmp);
    constexpr auto case2 = tunum::has_single_bit(0b0000'0000'0001'0000'0000'0000'0000'0000'0000'0000_ufmp);
    constexpr auto case3 = tunum::has_single_bit(0_ufmp);
    EXPECT_FALSE(case1);
    EXPECT_TRUE(case2);
    EXPECT_FALSE(case3);

    constexpr auto case4 = tunum::bit_width(0_ufmp);
    constexpr auto case5 = tunum::bit_width(0b0000'0000'1001'0000'0000'0000'0000'0000'0000'0000_ufmp);
    constexpr auto case6 = tunum::bit_width(0b0000'0000'0001'0000'0000'0000'0000'0000'0000'0000_ufmp);
    EXPECT_EQ(case4, 0u);
    EXPECT_EQ(case5, 32);
    EXPECT_EQ(case6, 29);

    constexpr auto case7 = tunum::bit_ceil(0_ufmp);
    constexpr auto case8 = tunum::bit_ceil(0b0000'0000'1001'0000'0000'0000'0000'0000'0000'0000_ufmp);
    constexpr auto case9 = tunum::bit_ceil(0b0000'0000'0001'0000'0000'0000'0000'0000'0000'0000_ufmp);
    EXPECT_EQ(case7, 1u);
    EXPECT_EQ(case8, 0b0000'0001'0000'0000'0000'0000'0000'0000'0000'0000_ufmp);
    EXPECT_EQ(case9, 0b0000'0000'0001'0000'0000'0000'0000'0000'0000'0000_ufmp);

    constexpr auto case10 = tunum::bit_floor(0_ufmp);
    constexpr auto case11 = tunum::bit_floor(0b0000'0000'1001'0000'0000'0000'0000'0000'0000'0000_ufmp);
    constexpr auto case12 = tunum::bit_floor(0b0000'0000'0001'0000'0000'0000'0000'0000'0000'0000_ufmp);
    EXPECT_EQ(case10, 0u);
    EXPECT_EQ(case11, 0b0000'0000'1000'0000'0000'0000'0000'0000'0000'0000_ufmp);
    EXPECT_EQ(case12, 0b0000'0000'0001'0000'0000'0000'0000'0000'0000'0000_ufmp);
}
