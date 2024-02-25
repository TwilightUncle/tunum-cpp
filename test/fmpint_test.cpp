#include <gtest/gtest.h>
#include <tunum/fmpint.hpp>

using int128_t_2 = tunum::fmpint<15>;
using int64_t_1 = tunum::fmpint<0>;
using int64_t_2 = tunum::fmpint<8>;
constexpr auto uint32_max = ~std::uint32_t{};

TEST(TunumFmpintTest, ConstructorTest)
{

    ASSERT_EQ(tunum::int128_t::size, int128_t_2::size);
    ASSERT_EQ(int64_t_1::size, int64_t_2::size);

    // デフォルトコンストラクタ呼び出しの確認
    constexpr auto v1 = int128_t_2{};
    constexpr auto v2 = int64_t_2{};

    // 組み込み整数による初期化
    constexpr auto v3 = int128_t_2{std::int32_t(1234)};
    constexpr auto v4 = int64_t_2{std::int32_t(-5678)};
    constexpr auto v5 = int128_t_2{~std::uint64_t{}};
    constexpr auto v6 = int64_t_2{~std::uint64_t{} << 2};

    ASSERT_EQ(v3.lower.lower, 1234);
    ASSERT_EQ(v3.lower.upper, 0);
    ASSERT_EQ(v3.sign, false);
    ASSERT_EQ(v4.lower, 5678);
    ASSERT_EQ(v4.upper, 0);
    ASSERT_EQ(v4.sign, true);
    ASSERT_TRUE(v5.lower.lower == uint32_max);
    ASSERT_TRUE(v5.lower.upper == uint32_max);
    ASSERT_TRUE(v5.upper.lower == 0);
    ASSERT_EQ(v6.lower, uint32_max << 2);
    ASSERT_EQ(v6.upper, uint32_max);

    // fmpintによる初期化
    constexpr auto v7 = int128_t_2{v3};         // 同じ型
    constexpr auto v8 = tunum::int128_t{v3};    // Bytesが異なるが内部的に同じ表現の型
    constexpr auto v9 = int128_t_2{v4};         // 小さいサイズ -> 大きいサイズ
    constexpr auto v10 = int64_t_1{v5};         // 大きいサイズ -> 小さいサイズ

    ASSERT_EQ(v7.lower.lower, 1234);
    ASSERT_EQ(v7.lower.upper, 0);
    ASSERT_EQ(v7.sign, false);
    ASSERT_EQ(v8.lower.lower, 1234);
    ASSERT_EQ(v8.lower.upper, 0);
    ASSERT_EQ(v8.sign, false);
    ASSERT_EQ(v9.lower.lower, 5678);
    ASSERT_EQ(v9.lower.upper, 0);
    ASSERT_EQ(v9.sign, true);
    ASSERT_EQ(v10.lower, uint32_max);
    ASSERT_EQ(v10.upper, uint32_max);
    ASSERT_EQ(v10.sign, false);
}

TEST(TunumFmpintTest, ElementAccessTest)
{
    auto v1 = tunum::int128_t{(~std::uint64_t{}) << 2};
    v1.upper = 5;

    ASSERT_EQ(v1.at(0), uint32_max << 2);
    ASSERT_EQ(v1.at(0), v1.lower.lower);
    ASSERT_EQ(v1.at(1), uint32_max);
    ASSERT_EQ(v1.at(1), v1.lower.upper);
    ASSERT_EQ(v1.at(2), 5);
    ASSERT_EQ(v1.at(2), v1.upper.lower);
    ASSERT_EQ(v1.at(3), 0);
    ASSERT_EQ(v1.at(3), v1.upper.upper);
    ASSERT_THROW(v1.at(4), std::out_of_range);
    ASSERT_THROW(v1.at(5), std::out_of_range);
}
