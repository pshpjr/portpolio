#include "utils/random_utils.h"

#include <vector>

#include <gtest/gtest.h>

namespace psh::lib::utils
{
namespace
{

TEST(RandomUtilsTest, RandomEngineUsesInclusiveIntAndHalfOpenFloatRanges)
{
    RandomEngine engine(1234);

    for (int i = 0; i < 32; ++i)
    {
        const int intValue = engine.rand_int(3, 5);
        const double floatValue = engine.rand_float(1.5, 2.5);

        EXPECT_GE(intValue, 3);
        EXPECT_LE(intValue, 5);
        EXPECT_GE(floatValue, 1.5);
        EXPECT_LT(floatValue, 2.5);
    }
}

TEST(RandomUtilsTest, RandomEngineSupportsRangeObjectsAndProbabilityEdges)
{
    RandomEngine engine(77);

    EXPECT_EQ(engine.rand_int(IntRange{4, 4}), 4);
    EXPECT_DOUBLE_EQ(engine.rand_float(FloatRange{2.0, 2.0}), 2.0);
    EXPECT_FALSE(engine.rand_bool(0.0));
    EXPECT_TRUE(engine.rand_bool(1.0));
}

TEST(RandomUtilsTest, RandPickAndShuffleUseEngineState)
{
    RandomEngine first(42);
    RandomEngine second(42);
    std::vector<int> firstValues{1, 2, 3, 4, 5};
    std::vector<int> secondValues{1, 2, 3, 4, 5};

    EXPECT_EQ(first.rand_pick(firstValues), second.rand_pick(secondValues));

    first.shuffle(firstValues);
    second.shuffle(secondValues);

    EXPECT_EQ(firstValues, secondValues);
}

TEST(RandomUtilsTest, GlobalRandHelpersRespectProvidedBounds)
{
    for (int i = 0; i < 32; ++i)
    {
        const int value = Rand(IntRange{10, 12});
        const double real = Rand(FloatRange{0.0, 1.0});

        EXPECT_GE(value, 10);
        EXPECT_LE(value, 12);
        EXPECT_GE(real, 0.0);
        EXPECT_LT(real, 1.0);
    }
}

} // namespace
} // namespace psh::lib::utils
