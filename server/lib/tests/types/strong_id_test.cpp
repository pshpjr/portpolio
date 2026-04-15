#include "types/strong_id.h"

#include <concepts>
#include <cstdint>
#include <type_traits>
#include <unordered_map>

#include <gtest/gtest.h>

namespace psh::lib::types
{
namespace
{

struct ContentItemTag;
struct SkillTag;

using ContentItemId = StrongId<ContentItemTag>;
using SkillId = StrongId<SkillTag>;
using LargeContentItemId = StrongId<ContentItemTag, std::int64_t>;

TEST(StrongIdTest, StoresAndReturnsExplicitValue)
{
    const ContentItemId id(42);

    EXPECT_EQ(ValueOf(id), 42);
}

TEST(StrongIdTest, SupportsComparisonWithinSameTag)
{
    const ContentItemId low(1);
    const ContentItemId high(5);

    EXPECT_LT(ValueOf(low), ValueOf(high));
    EXPECT_NE(low, high);
    EXPECT_EQ(ContentItemId(5), high);
}

TEST(StrongIdTest, CanBeUsedAsUnorderedMapKey)
{
    std::unordered_map<ContentItemId, int> counts;

    counts.emplace(ContentItemId(7), 3);

    EXPECT_EQ(counts.at(ContentItemId(7)), 3);
}

TEST(StrongIdTest, SupportsAlternateIntegralStorageTypes)
{
    const LargeContentItemId id(99);

    static_assert(std::is_same_v<strong::underlying_type_t<LargeContentItemId>, std::int64_t>);
    EXPECT_EQ(ValueOf(id), 99);
}

static_assert(std::is_default_constructible_v<ContentItemId>);
static_assert(std::is_constructible_v<ContentItemId, int>);
static_assert(!std::is_convertible_v<int, ContentItemId>);
static_assert(!std::equality_comparable_with<ContentItemId, SkillId>);
static_assert(!std::three_way_comparable_with<ContentItemId, SkillId>);

} // namespace
} // namespace psh::lib::types
