#include <gtest/gtest.h>
#include <tunum/fmpint.hpp>

using int128_t_2 = tunum::fmpint<15>;
using int64_t_1 = tunum::fmpint<0>;
using int64_t_2 = tunum::fmpint<8>;

constexpr auto bit32_1 = 0b0000'1111'1111'1111'1111'0000'0000'1111u;
constexpr auto bit32_2 = 0b0000'1111'1111'1111'0000'0000'0000'0000u;
constexpr auto bit32_3 = std::uint32_t{};
constexpr auto bit32_4 = ~std::uint32_t{};
constexpr auto bit64_lit_1 = 0b0000'1111'1111'1111'1111'0000'0000'1111'0000'1111'1111'1111'0000'0000'0000'0000u;

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
    ASSERT_EQ(v4.lower, 5678);
    ASSERT_EQ(v4.upper, 0);
    ASSERT_TRUE(v5.lower.lower == bit32_4);
    ASSERT_TRUE(v5.lower.upper == bit32_4);
    ASSERT_TRUE(v5.upper.lower == 0);
    ASSERT_EQ(v6.lower, bit32_4 << 2);
    ASSERT_EQ(v6.upper, bit32_4);

    // fmpintによる初期化
    constexpr auto v7 = int128_t_2{v3};         // 同じ型
    constexpr auto v8 = tunum::int128_t{v3};    // Bytesが異なるが内部的に同じ表現の型
    constexpr auto v9 = int128_t_2{v4};         // 小さいサイズ -> 大きいサイズ
    constexpr auto v10 = int64_t_1{v5};         // 大きいサイズ -> 小さいサイズ

    ASSERT_EQ(v7.lower.lower, 1234);
    ASSERT_EQ(v7.lower.upper, 0);
    ASSERT_EQ(v8.lower.lower, 1234);
    ASSERT_EQ(v8.lower.upper, 0);
    ASSERT_EQ(v9.lower.lower, 5678);
    ASSERT_EQ(v9.lower.upper, 0);
    ASSERT_EQ(v10.lower, bit32_4);
    ASSERT_EQ(v10.upper, bit32_4);
}

TEST(TunumFmpintTest, ElementAccessTest)
{
    auto v1 = tunum::int128_t{(~std::uint64_t{}) << 2};
    v1.upper = 5;

    ASSERT_EQ(v1.at(0), bit32_4 << 2);
    ASSERT_EQ(v1.at(0), v1.lower.lower);
    ASSERT_EQ(v1.at(1), bit32_4);
    ASSERT_EQ(v1.at(1), v1.lower.upper);
    ASSERT_EQ(v1.at(2), 5);
    ASSERT_EQ(v1.at(2), v1.upper.lower);
    ASSERT_EQ(v1.at(3), 0);
    ASSERT_EQ(v1.at(3), v1.upper.upper);
    ASSERT_THROW(v1.at(4), std::out_of_range);
    ASSERT_THROW(v1.at(5), std::out_of_range);

    v1.at(2) = 1000;
    ASSERT_EQ(v1.at(2), 1000);
}

TEST(TunumFmpintTest, BitRotateTest)
{
    auto bit128_1 = tunum::int128_t{};
    bit128_1[0] = bit32_1;
    bit128_1[1] = bit32_2;
    bit128_1[2] = bit32_3;
    bit128_1[3] = bit32_4;

    // 結果が変化しないことの確認
    for (int i = 0; i < 6; i++) {
        const auto bit128_n = bit128_1.rotate_l(i * 128);
        EXPECT_EQ(bit128_n[0], bit32_1);
        EXPECT_EQ(bit128_n[1], bit32_2);
    }
    for (int i = 0; i < 6; i++) {
        const auto bit128_n = bit128_1.rotate_r(i * 128);
        EXPECT_EQ(bit128_n[2], bit32_3);
        EXPECT_EQ(bit128_n[3], bit32_4);
    }
    const auto bit128_2 = bit128_1.rotate_l(64);
    EXPECT_EQ(bit128_2[0], bit32_3);
    EXPECT_EQ(bit128_2[1], bit32_4);
    EXPECT_EQ(bit128_2[2], bit32_1);
    EXPECT_EQ(bit128_2[3], bit32_2);
    const auto bit128_3 = bit128_2.rotate_r(32);
    EXPECT_EQ(bit128_3[0], bit32_4);
    EXPECT_EQ(bit128_3[1], bit32_1);
    EXPECT_EQ(bit128_3[2], bit32_2);
    EXPECT_EQ(bit128_3[3], bit32_3);
    const auto bit128_4 = bit128_3.rotate_l(112 + 128 * 9);
    EXPECT_EQ(bit128_4[0], 0b1111'0000'0000'1111'1111'1111'1111'1111u);
    EXPECT_EQ(bit128_4[1], 0b0000'0000'0000'0000'0000'1111'1111'1111u);
    EXPECT_EQ(bit128_4[2], 0b0000'0000'0000'0000'0000'1111'1111'1111u);
    EXPECT_EQ(bit128_4[3], 0b1111'1111'1111'1111'0000'0000'0000'0000u);
    const auto bit128_5 = bit128_4.rotate_r(16 + 128 * 54);
    EXPECT_EQ(bit128_5[0], bit32_1);
    EXPECT_EQ(bit128_5[1], bit32_2);
    EXPECT_EQ(bit128_5[2], bit32_3);
    EXPECT_EQ(bit128_5[3], bit32_4);

    // constexpr でも動くか確認
    constexpr auto bit64_1 = tunum::fmpint<8>{0b0000'1111'1111'1111'1111'0000'0000'1111'0000'1111'1111'1111'0000'0000'0000'0000u};
    constexpr auto bit64_2 = bit64_1.rotate_l(48);
    EXPECT_EQ(bit64_2[0], 0b1111'0000'0000'1111'0000'1111'1111'1111u);
    EXPECT_EQ(bit64_2[1], 0b0000'0000'0000'0000'0000'1111'1111'1111u);
    constexpr auto bit64_3 = bit64_2.rotate_r(48 + 64 * 123);
    EXPECT_EQ(bit64_3[0], bit32_2);
    EXPECT_EQ(bit64_3[1], bit32_1);

    auto bit256_1 = tunum::int256_t{};
    bit256_1[0] = bit32_1;
    bit256_1[1] = bit32_2;
    bit256_1[2] = bit32_3;
    bit256_1[3] = bit32_4;
    bit256_1[4] = bit32_3;
    bit256_1[5] = bit32_2;
    bit256_1[6] = bit32_1;
    bit256_1[7] = bit32_4;
    const auto bit256_2 = bit256_1.rotate_l(128 + 11)
        .rotate_r(128 + 11 + 256 * 1237);
    EXPECT_EQ(bit256_2[0], bit32_1);
    EXPECT_EQ(bit256_2[1], bit32_2);
    EXPECT_EQ(bit256_2[2], bit32_3);
    EXPECT_EQ(bit256_2[3], bit32_4);
    EXPECT_EQ(bit256_2[4], bit32_3);
    EXPECT_EQ(bit256_2[5], bit32_2);
    EXPECT_EQ(bit256_2[6], bit32_1);
    EXPECT_EQ(bit256_2[7], bit32_4);
}

// const 修飾のついたメンバ関数の演算子オーバーロードテスト
// ※内部的に四則演算のオーバーロードを用いるものを除く
TEST(TunumFmpintTest, ConstMemberOperatorTest)
{
    EXPECT_FALSE(bool(tunum::int128_t{}));
    EXPECT_TRUE(!tunum::int128_t{});
    EXPECT_TRUE((bool)tunum::int256_t{1});
    EXPECT_FALSE(!tunum::int256_t{1});

    constexpr auto completion_1 = ~int128_t_2{};
    for (int i = 0; i < 4; i++)
        EXPECT_EQ(completion_1.at(i), ~std::uint32_t{});
    EXPECT_TRUE(static_cast<bool>(completion_1));
    EXPECT_FALSE(!completion_1);

    constexpr auto completion_2 = ~tunum::int128_t{bit32_4};
    EXPECT_EQ(completion_2[0], bit32_3);
    EXPECT_EQ(completion_2[1], bit32_4);
    EXPECT_EQ(completion_2[2], bit32_4);
    EXPECT_EQ(completion_2[3], bit32_4);

    constexpr auto completion_3 = ~completion_2;
    EXPECT_EQ(completion_3[0], bit32_4);
    EXPECT_EQ(completion_3[1], bit32_3);
    EXPECT_EQ(completion_3[2], bit32_3);
    EXPECT_EQ(completion_3[3], bit32_3);

    constexpr auto v1 = int128_t_2{2} + int64_t_2{1};
    constexpr auto v2 = int64_t_2{2} + int128_t_2{1};
    static_assert(std::same_as<decltype(v1), decltype(v2)>, "expect same type.");
    
    for (int i = 0; i < 4; i++)
        ASSERT_EQ(v1.at(i), v2.at(i));
}

TEST(TunumFmpintTest, BitOperationTest)
{
    EXPECT_EQ(tunum::int128_t{}.countl_zero_bit(), 128);
    EXPECT_EQ(tunum::int128_t{}.countl_one_bit(), 0);
    EXPECT_EQ(tunum::int128_t{}.countr_zero_bit(), 128);
    EXPECT_EQ(tunum::int128_t{}.countr_one_bit(), 0);
    EXPECT_EQ(tunum::int128_t{}.count_zero_bit(), 128);
    EXPECT_EQ(tunum::int128_t{}.count_one_bit(), 0);
    EXPECT_EQ(tunum::int128_t{}.is_full_bit(), false);
    EXPECT_EQ(tunum::int128_t{}.is_full_bit(false), true);
    EXPECT_EQ(tunum::int128_t{}.get_bit_width(), 0);
    EXPECT_EQ(tunum::int128_t{}.get_bit(0), false);
    EXPECT_EQ(tunum::int128_t{}.get_bit(127), false);

    constexpr auto bit256_1 = ~tunum::int256_t{};
    EXPECT_EQ(bit256_1.countl_zero_bit(), 0);
    EXPECT_EQ(bit256_1.countl_one_bit(), 256);
    EXPECT_EQ(bit256_1.countr_zero_bit(), 0);
    EXPECT_EQ(bit256_1.countr_one_bit(), 256);
    EXPECT_EQ(bit256_1.count_zero_bit(), 0);
    EXPECT_EQ(bit256_1.count_one_bit(), 256);
    EXPECT_EQ(bit256_1.is_full_bit(), true);
    EXPECT_EQ(bit256_1.is_full_bit(false), false);
    EXPECT_EQ(bit256_1.get_bit_width(), 256);
    EXPECT_EQ(bit256_1.get_bit(0), true);
    EXPECT_EQ(bit256_1.get_bit(255), true);

    constexpr auto bit64_1 = tunum::fmpint<8>{bit64_lit_1};
    EXPECT_EQ(bit64_1.countl_zero_bit(), 4);
    EXPECT_EQ(bit64_1.countl_one_bit(), 0);
    EXPECT_EQ(bit64_1.countr_zero_bit(), 16);
    EXPECT_EQ(bit64_1.countr_one_bit(), 0);
    EXPECT_EQ(bit64_1.count_zero_bit(), 32);
    EXPECT_EQ(bit64_1.count_one_bit(), 32);
    EXPECT_EQ(bit64_1.is_full_bit(), false);
    EXPECT_EQ(bit64_1.is_full_bit(false), false);
    EXPECT_EQ(bit64_1.get_bit_width(), 60);
    EXPECT_EQ(bit64_1.get_bit(0), false);
    EXPECT_EQ(bit64_1.get_bit(15), false);
    EXPECT_EQ(bit64_1.get_bit(16), true);
    EXPECT_EQ(bit64_1.get_bit(59), true);
    EXPECT_EQ(bit64_1.get_bit(60), false);
    EXPECT_EQ(bit64_1.get_bit(63), false);

    constexpr auto bit512_1 = tunum::int512_t{~bit64_1};
    EXPECT_EQ(bit512_1.countl_zero_bit(), 512 - 64);
    EXPECT_EQ(bit512_1.countl_one_bit(), 0);
    EXPECT_EQ(bit512_1.countr_zero_bit(), 0);
    EXPECT_EQ(bit512_1.countr_one_bit(), 16);
    EXPECT_EQ(bit512_1.count_zero_bit(), 512 - 32);
    EXPECT_EQ(bit512_1.count_one_bit(), 32);
    EXPECT_EQ(bit512_1.is_full_bit(), false);
    EXPECT_EQ(bit512_1.is_full_bit(false), false);
    EXPECT_EQ(bit512_1.get_bit_width(), 64);

    auto bit128_1 = tunum::int128_t{};
    bit128_1.set_bit(5, true);
    bit128_1.set_bit(43, true);
    bit128_1.set_bit(44, true);
    bit128_1.set_bit(44, false);
    EXPECT_EQ(bit128_1[0], 1 << 5);
    EXPECT_EQ(bit128_1[1], 1 << (43 - 32));
}
