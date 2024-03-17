#include <gtest/gtest.h>
#include <tunum/uint_base_number.hpp>

TEST(TunumUintBaseNumberTest, ToIntegerTest)
{
    constexpr auto pow = [](std::size_t v, std::size_t index) -> std::size_t {
        std::size_t r = 1;
        for (std::size_t i = 0; i < index; i++)
            r *= v;
        return r;
    };
    constexpr auto nums_1 = std::array<std::uint8_t, 5> { 0, 2, 4, 1, 3 };
    ASSERT_THROW(tunum::uibn::convert_numbers_to_integer<4>(nums_1), std::invalid_argument);

    constexpr auto converted_1 = tunum::uibn::convert_numbers_to_integer<5>(nums_1);
    EXPECT_EQ(converted_1, 0 + (2 * 5) + (4 * pow(5, 2)) + (1 * pow(5, 3)) + (3 * pow(5, 4)));
    constexpr auto converted_2 = tunum::uibn::convert_numbers_to_integer<7>(nums_1);
    EXPECT_EQ(converted_2, 0 + (2 * 7) + (4 * pow(7, 2)) + (1 * pow(7, 3)) + (3 * pow(7, 4)));
}
