#include "utils/container_utils.h"

#include <map>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace psh::lib::utils {
namespace {

TEST(ContainerUtilsTest, ContainsAndFindIfOrInspectRanges) {
    const std::vector<int> values{2, 4, 6, 8};

    EXPECT_TRUE(contains(values, 6));
    EXPECT_FALSE(contains(values, 5));
    EXPECT_EQ(find_if_or(values, [](const int value) { return value > 5; }, -1),
              6);
    EXPECT_EQ(find_if_or(values, [](const int value) { return value > 10; }, -1),
              -1);
}

TEST(ContainerUtilsTest, EraseIfRemovesMatchingElements) {
    std::vector<int> values{1, 2, 3, 4, 5, 6};

    const auto removed = erase_if(values, [](const int value) {
        return value % 2 == 0;
    });

    EXPECT_EQ(removed, 3);
    EXPECT_EQ(values, (std::vector<int>{1, 3, 5}));
}

TEST(ContainerUtilsTest, TransformToVecCollectsProjectedValues) {
    const std::vector<int> values{1, 2, 3};

    const auto transformed =
        transform_to_vec(values, [](const int value) { return value * value; });

    EXPECT_EQ(transformed, (std::vector<int>{1, 4, 9}));
}

TEST(ContainerUtilsTest, KeysAndValuesExposeMapViews) {
    const std::map<int, std::string> table{{1, "one"}, {2, "two"}, {3, "three"}};

    const auto keyList = transform_to_vec(keys(table), [](const auto& key) {
        return key;
    });
    const auto valueList =
        transform_to_vec(values(table), [](const auto& value) { return value; });

    EXPECT_EQ(keyList, (std::vector<int>{1, 2, 3}));
    EXPECT_EQ(valueList, (std::vector<std::string>{"one", "two", "three"}));
}

}  // namespace
}  // namespace psh::lib::utils
