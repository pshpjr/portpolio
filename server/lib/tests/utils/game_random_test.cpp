#include "utils/game_random.h"

#include <algorithm>
#include <vector>

#include <gtest/gtest.h>

namespace psh::lib::utils
{
namespace
{

TEST(GameRandomTest, BagDrawConsumesConfiguredWeightsWithinCycle)
{
    BagDraw<int> draw({{1, 3}, {2, 1}, {3, 1}});
    std::vector<int> picks;

    for (int i = 0; i < 5; ++i)
    {
        picks.push_back(draw.draw());
    }

    EXPECT_EQ(draw.remaining(), 0);
    EXPECT_EQ(static_cast<int>(std::count(picks.begin(), picks.end(), 1)), 3);
    EXPECT_EQ(static_cast<int>(std::count(picks.begin(), picks.end(), 2)), 1);
    EXPECT_EQ(static_cast<int>(std::count(picks.begin(), picks.end(), 3)), 1);
}

TEST(GameRandomTest, BagDrawCanResumeFromSavedState)
{
    BagDraw<int> original({{10, 2}, {20, 1}});

    const int firstDraw = original.draw();
    const auto state = original.save_state();

    BagDraw<int> restored({{10, 2}, {20, 1}}, state);

    EXPECT_TRUE(firstDraw == 10 || firstDraw == 20);
    EXPECT_EQ(restored.remaining(), original.remaining());
    EXPECT_EQ(restored.draw(), original.draw());
}

TEST(GameRandomTest, BagDrawStartsNewCycleAfterBagIsExhausted)
{
    BagDraw<int> draw({{7, 1}});

    EXPECT_EQ(draw.draw(), 7);
    EXPECT_EQ(draw.remaining(), 0);
    EXPECT_EQ(draw.draw(), 7);
    EXPECT_EQ(draw.remaining(), 0);
}

} // namespace
} // namespace psh::lib::utils
