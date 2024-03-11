#include <gtest/gtest.h>
#include <tunum/fmpint.hpp>

using uint128_t_2 = tunum::fmpint<15, false>;
using uint64_t_1 = tunum::fmpint<0, false>;
using uint64_t_2 = tunum::fmpint<8, false>;

constexpr auto bit32_1 = 0b0000'1111'1111'1111'1111'0000'0000'1111u;
constexpr auto bit32_2 = 0b0000'1111'1111'1111'0000'0000'0000'0000u;
constexpr auto bit32_3 = std::uint32_t{};
constexpr auto bit32_4 = ~std::uint32_t{};
constexpr auto bit64_lit_1 = 0b0000'1111'1111'1111'1111'0000'0000'1111'0000'1111'1111'1111'0000'0000'0000'0000u;

TEST(TunumFmpintTest, ConstructorTest)
{
    ASSERT_EQ(tunum::uint128_t::size, uint128_t_2::size);
    ASSERT_EQ(uint64_t_1::size, uint64_t_2::size);

    // デフォルトコンストラクタ呼び出しの確認
    constexpr auto v1 = uint128_t_2{};
    constexpr auto v2 = uint64_t_2{};

    // 組み込み整数による初期化
    constexpr auto v3 = uint128_t_2{std::int32_t(1234)};
    constexpr auto v4 = uint64_t_2{std::int32_t(-5678)};
    constexpr auto v5 = uint128_t_2{~std::uint64_t{}};
    constexpr auto v6 = uint64_t_2{~std::uint64_t{} << 2};

    ASSERT_EQ(v3.lower.lower, 1234);
    ASSERT_EQ(v3.lower.upper, 0);
    ASSERT_EQ(v4.lower, -5678);
    ASSERT_EQ(v4.upper, bit32_4);
    ASSERT_TRUE(v5.lower.lower == bit32_4);
    ASSERT_TRUE(v5.lower.upper == bit32_4);
    ASSERT_TRUE(v5.upper.lower == 0);
    ASSERT_EQ(v6.lower, bit32_4 << 2);
    ASSERT_EQ(v6.upper, bit32_4);

    // fmpintによる初期化
    constexpr auto v7 = uint128_t_2{v3};         // 同じ型
    constexpr auto v8 = tunum::uint128_t{v3};    // Bytesが異なるが内部的に同じ表現の型
    constexpr auto v9 = uint128_t_2{v4};         // 小さいサイズ -> 大きいサイズ
    constexpr auto v10 = uint64_t_1{v5};         // 大きいサイズ -> 小さいサイズ

    ASSERT_EQ(v7.lower.lower, 1234);
    ASSERT_EQ(v7.lower.upper, 0);
    ASSERT_EQ(v8.lower.lower, 1234);
    ASSERT_EQ(v8.lower.upper, 0);
    ASSERT_EQ(v9.lower.lower, -5678);
    ASSERT_EQ(v9.lower.upper, bit32_4);
    ASSERT_EQ(v10.lower, bit32_4);
    ASSERT_EQ(v10.upper, bit32_4);
}

TEST(TunumFmpintTest, ElementAccessTest)
{
    auto v1 = tunum::uint128_t{(~std::uint64_t{}) << 2};
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
    auto bit128_1 = tunum::uint128_t{};
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
    constexpr auto bit64_1 = uint64_t_2{0b0000'1111'1111'1111'1111'0000'0000'1111'0000'1111'1111'1111'0000'0000'0000'0000u};
    constexpr auto bit64_2 = bit64_1.rotate_l(48);
    EXPECT_EQ(bit64_2[0], 0b1111'0000'0000'1111'0000'1111'1111'1111u);
    EXPECT_EQ(bit64_2[1], 0b0000'0000'0000'0000'0000'1111'1111'1111u);
    constexpr auto bit64_3 = bit64_2.rotate_r(48 + 64 * 123);
    EXPECT_EQ(bit64_3[0], bit32_2);
    EXPECT_EQ(bit64_3[1], bit32_1);

    auto bit256_1 = tunum::uint256_t{};
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

    // シフト演算子
    constexpr auto bit64_4 = bit64_1 << 33;
    EXPECT_EQ(bit64_4[0], 0);
    EXPECT_EQ(bit64_4[1], 0b0001'1111'1111'1110'0000'0000'0000'0000u);
    constexpr auto bit64_5 = bit64_4 >> 60;
    EXPECT_EQ(bit64_5[0], 1);
    EXPECT_EQ(bit64_5[1], 0);

    const auto bit128_6 = bit128_4 << (64 + 4);
    EXPECT_EQ(bit128_6[0], 0);
    EXPECT_EQ(bit128_6[1], 0);
    EXPECT_EQ(bit128_6[2], 0b0000'0000'1111'1111'1111'1111'1111'0000u);
    EXPECT_EQ(bit128_6[3], 0b0000'0000'0000'0000'1111'1111'1111'1111u);
    const auto bit128_7 = bit128_6 >> (32 * 3 + 4 * 3);
    EXPECT_EQ(bit128_7[0], 0b1111u);
    EXPECT_EQ(bit128_7[1], 0);
    EXPECT_EQ(bit128_7[2], 0);
    EXPECT_EQ(bit128_7[3], 0);
}

// イミュータブルな演算子オーバーロードテスト
// ※内部的に四則演算のオーバーロードを用いるものを除く
TEST(TunumFmpintTest, ConstOperatorTest)
{
    constexpr auto v1 = bool(tunum::uint128_t{});
    EXPECT_FALSE(v1);
    constexpr auto v2 = !tunum::uint128_t{};
    EXPECT_TRUE(v2);
    EXPECT_TRUE((bool)tunum::uint256_t{1});
    EXPECT_FALSE(!tunum::uint256_t{1});

    constexpr auto completion_1 = ~uint128_t_2{};
    for (int i = 0; i < 4; i++)
        EXPECT_EQ(completion_1.at(i), ~std::uint32_t{});
    EXPECT_TRUE(static_cast<bool>(completion_1));
    EXPECT_FALSE(!completion_1);

    constexpr auto completion_2 = ~tunum::uint128_t{bit32_4};
    EXPECT_EQ(completion_2[0], bit32_3);
    EXPECT_EQ(completion_2[1], bit32_4);
    EXPECT_EQ(completion_2[2], bit32_4);
    EXPECT_EQ(completion_2[3], bit32_4);

    constexpr auto completion_3 = ~completion_2;
    EXPECT_EQ(completion_3[0], bit32_4);
    EXPECT_EQ(completion_3[1], bit32_3);
    EXPECT_EQ(completion_3[2], bit32_3);
    EXPECT_EQ(completion_3[3], bit32_3);

    EXPECT_TRUE(tunum::uint128_t{} == tunum::uint256_t{});
    EXPECT_TRUE(tunum::uint128_t{} <= tunum::uint256_t{});
    EXPECT_FALSE(tunum::uint128_t{} > tunum::uint256_t{});
    EXPECT_TRUE(~tunum::uint128_t{} != ~tunum::uint256_t{});
    EXPECT_FALSE(~tunum::uint128_t{} >= ~tunum::uint256_t{});
    EXPECT_TRUE(~tunum::uint128_t{} < ~tunum::uint256_t{});
    EXPECT_TRUE(tunum::uint128_t{} != 1);
    EXPECT_TRUE(tunum::uint128_t{} < 1);
    EXPECT_TRUE(4 > tunum::uint256_t{3});
    EXPECT_TRUE(4 == tunum::uint256_t{4});
    constexpr auto l_v = tunum::int256_t{-5};
    constexpr auto r_v = tunum::uint128_t{-5};
    constexpr std::uint64_t a = std::int8_t(-1);
    EXPECT_TRUE(tunum::int256_t{-5} != tunum::uint128_t{-5});
}

TEST(TunumFmpintTest, BitOperationTest)
{
    EXPECT_EQ(tunum::uint128_t{}.countl_zero_bit(), 128);
    EXPECT_EQ(tunum::uint128_t{}.countl_one_bit(), 0);
    EXPECT_EQ(tunum::uint128_t{}.countr_zero_bit(), 128);
    EXPECT_EQ(tunum::uint128_t{}.countr_one_bit(), 0);
    EXPECT_EQ(tunum::uint128_t{}.count_zero_bit(), 128);
    EXPECT_EQ(tunum::uint128_t{}.count_one_bit(), 0);
    EXPECT_EQ(tunum::uint128_t{}.is_full_bit(), false);
    EXPECT_EQ(tunum::uint128_t{}.is_full_bit(false), true);
    EXPECT_EQ(tunum::uint128_t{}.get_bit_width(), 0);
    EXPECT_EQ(tunum::uint128_t{}.get_bit(0), false);
    EXPECT_EQ(tunum::uint128_t{}.get_bit(127), false);

    constexpr auto bit256_1 = ~tunum::uint256_t{};
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

    constexpr auto bit64_1 = uint64_t_2{bit64_lit_1};
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

    constexpr auto bit512_1 = tunum::uint512_t{~bit64_1};
    EXPECT_EQ(bit512_1.countl_zero_bit(), 512 - 64);
    EXPECT_EQ(bit512_1.countl_one_bit(), 0);
    EXPECT_EQ(bit512_1.countr_zero_bit(), 0);
    EXPECT_EQ(bit512_1.countr_one_bit(), 16);
    EXPECT_EQ(bit512_1.count_zero_bit(), 512 - 32);
    EXPECT_EQ(bit512_1.count_one_bit(), 32);
    EXPECT_EQ(bit512_1.is_full_bit(), false);
    EXPECT_EQ(bit512_1.is_full_bit(false), false);
    EXPECT_EQ(bit512_1.get_bit_width(), 64);

    auto bit128_1 = tunum::uint128_t{};
    bit128_1.set_bit(5, true);
    bit128_1.set_bit(43, true);
    bit128_1.set_bit(43, true);
    bit128_1.set_bit(44, true);
    bit128_1.set_bit(44, false);
    bit128_1.set_bit(44, false);
    EXPECT_EQ(bit128_1[0], 1 << 5);
    EXPECT_EQ(bit128_1[1], 1 << (43 - 32));
}

TEST(TunumFmpintTest, OperatorTest)
{
    // 加算
    constexpr auto v1 = uint128_t_2{2} + uint64_t_2{1};
    constexpr auto v2 = uint64_t_2{2} + uint128_t_2{1};
    static_assert(std::same_as<decltype(v1), decltype(v2)>, "expect same type.");
    
    for (int i = 0; i < 4; i++)
        ASSERT_EQ(v1.at(i), v2.at(i));

    constexpr auto v3 = tunum::uint128_t{~std::uint32_t{}} + tunum::uint128_t{2};
    EXPECT_EQ(v3[0], 1);
    EXPECT_EQ(v3[1], 1);
    EXPECT_EQ(v3[2], 0);
    EXPECT_EQ(v3[3], 0);
    constexpr auto v4 = tunum::uint128_t{~std::uint32_t{}} + ~std::uint32_t{};
    EXPECT_EQ(v4[0], ~std::uint32_t{} - 1);
    EXPECT_EQ(v4[1], 1);
    EXPECT_EQ(v4[2], 0);
    EXPECT_EQ(v4[3], 0);
    constexpr auto v5 = (~tunum::uint256_t{} >> 32) + (~tunum::uint256_t{} >> 32);
    EXPECT_EQ(v5[0], ~std::uint32_t{} - 1);
    EXPECT_EQ(v5[1], ~std::uint32_t{});
    EXPECT_EQ(v5[2], ~std::uint32_t{});
    EXPECT_EQ(v5[3], ~std::uint32_t{});
    EXPECT_EQ(v5[4], ~std::uint32_t{});
    EXPECT_EQ(v5[5], ~std::uint32_t{});
    EXPECT_EQ(v5[6], ~std::uint32_t{});
    EXPECT_EQ(v5[7], 1);

    // 減算
    constexpr auto v6 = v3 - 2;
    EXPECT_EQ(v6[0], ~std::uint32_t{});
    EXPECT_EQ(v6[1], 0);
    EXPECT_EQ(v6[2], 0);
    EXPECT_EQ(v6[3], 0);

    constexpr auto v7 = v5 - tunum::uint128_t{v5};
    EXPECT_EQ(v7[0], 0);
    EXPECT_EQ(v7[1], 0);
    EXPECT_EQ(v7[2], 0);
    EXPECT_EQ(v7[3], 0);
    EXPECT_EQ(v7[4], ~std::uint32_t{});
    EXPECT_EQ(v7[5], ~std::uint32_t{});
    EXPECT_EQ(v7[6], ~std::uint32_t{});
    EXPECT_EQ(v7[7], 1);

    EXPECT_EQ(
        uint64_t_2{v5} - tunum::uint128_t{~std::uint32_t{} - 1},
        tunum::uint128_t{~std::uint32_t{}} << 32
    );
    EXPECT_TRUE(~tunum::uint128_t{} != ~tunum::uint128_t{} - 1);
    EXPECT_TRUE(~tunum::uint128_t{} > ~tunum::uint128_t{} - 1);
    EXPECT_FALSE(~tunum::uint128_t{} <= ~tunum::uint128_t{} - 1);

    // 乗算
    constexpr auto v8 = tunum::uint128_t{~std::uint32_t{}} * tunum::uint128_t{~std::uint32_t{}}
        == std::uint64_t{~std::uint32_t{}} * std::uint64_t{~std::uint32_t{}};
    EXPECT_TRUE(v8);
    EXPECT_EQ(
        uint64_t_2{~std::uint64_t{}} * tunum::uint128_t{~std::uint64_t{}},
        (tunum::uint128_t{~std::uint64_t{} - 1} << 64) + 1
    );

    // 除算・剰余
    EXPECT_THROW(~tunum::uint512_t{} / 0, std::invalid_argument);
    EXPECT_THROW(~tunum::uint256_t{} % tunum::uint512_t{0}, std::invalid_argument);
    EXPECT_EQ(0 / tunum::uint256_t{12345}, 0);
    EXPECT_EQ(tunum::uint512_t{} % 4, 0);
    EXPECT_EQ(tunum::uint512_t{3} / 4, 0);
    EXPECT_EQ(tunum::uint512_t{3} % 4, 3);
    EXPECT_EQ(tunum::uint512_t{3} / 1, 3);
    EXPECT_EQ(tunum::uint512_t{3} % 1, 0);
    // 等しい値
    EXPECT_THROW(tunum::uint512_t{} / tunum::uint128_t{}, std::invalid_argument);
    constexpr auto v9 = tunum::uint512_t{~tunum::uint128_t{}} / ~tunum::uint128_t{};
    EXPECT_EQ(v9, 1);
    EXPECT_EQ(tunum::uint512_t{~tunum::uint128_t{}} % ~tunum::uint128_t{}, 0);
    // 委譲→組み込み演算子呼び出し
    EXPECT_EQ((tunum::uint128_t{12345} << 60) / (tunum::uint512_t{34} << 60), 12345 / 34);
    constexpr auto v10 = (tunum::uint128_t{12345} << 60) % (tunum::uint512_t{34} << 60);
    EXPECT_EQ(v10, tunum::uint128_t{12345 % 34} << 60);
    // コア部分確認(とりあえず具体的な数字がわからないので、割り切れる上、ビット操作で予想できるもののみ)
    // 残りのテストは文字列初期化可能になってから
    constexpr auto v11 = ~tunum::uint512_t{} / ~tunum::uint128_t{};
    EXPECT_EQ(v11[0], 1);
    EXPECT_EQ(v11[1], 0);
    EXPECT_EQ(v11[2], 0);
    EXPECT_EQ(v11[3], 0);
    EXPECT_EQ(v11[4], 1);
    EXPECT_EQ(v11[5], 0);
    EXPECT_EQ(v11[6], 0);
    EXPECT_EQ(v11[7], 0);
    EXPECT_EQ(v11[8], 1);
    EXPECT_EQ(v11[9], 0);
    EXPECT_EQ(v11[10], 0);
    EXPECT_EQ(v11[11], 0);
    EXPECT_EQ(v11[12], 1);
    EXPECT_EQ(v11[13], 0);
    EXPECT_EQ(v11[14], 0);
    EXPECT_EQ(v11[15], 0);
}

using namespace tunum::literals;

TEST(TunumFmpintTest, StringConstructorTest)
{
    constexpr auto uint32_max_s = L"4294967295";
    constexpr auto uint32_over_s = u8"4294967296";

    // 2の128乗
    constexpr auto uint128_max_s = u"340282366920938463463374607431768211455";
    constexpr auto uint128_over_s = U"340282366920938463463374607431768211456";

    constexpr auto v1 = tunum::uint128_t{"0"};
    EXPECT_FALSE(bool(v1));
    constexpr auto v2 = tunum::uint256_t{uint32_max_s};
    EXPECT_EQ(v2, ~std::uint32_t{});
    // constexpr auto v3 = tunum::uint512_t{uint32_over_s};
    EXPECT_EQ(tunum::uint512_t{uint32_over_s}, std::uint64_t{~std::uint32_t{}} + 1);
    EXPECT_EQ(tunum::uint128_t{uint128_max_s}, ~tunum::uint128_t{});
    EXPECT_EQ(tunum::uint128_t{uint128_over_s}, 0);

    constexpr auto fmp_int_v1 = 340282366920938463463374607431768211455_fmp;
    constexpr auto fmp_int_v2 = 340'282'366'920'938'463'463'374'607'431'768'211'455_ufmp;
    EXPECT_EQ(fmp_int_v1, fmp_int_v2);
    // constexpr auto fmp_int_v3 = 0xFFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF_ufmp;
    // EXPECT_EQ(fmp_int_v1, fmp_int_v3);
}
