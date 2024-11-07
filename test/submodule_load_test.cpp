#include <gtest/gtest.h>
#include <tunum/submodule_loader.hpp>

TEST(TunumSubmoduleLoadTest, TumpTest)
{
    constexpr bool t1 = std::same_as<
        tunum::tump::eval<
            tunum::tump::flip,
            tunum::tump::map,
            tunum::tump::list<int, float, bool>,
            tunum::tump::_apply,
            tunum::tump::add_const,
            tunum::tump::_dot,
            tunum::tump::add_pointer
        >,
        tunum::tump::list<int* const, float* const, bool* const>
    >;
    EXPECT_TRUE(t1);
}
