#include "utils/string_utils.h"

#include <array>
#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

namespace psh::lib::utils {
namespace {

TEST(StringUtilsTest, TrimRemovesLeadingAndTrailingWhitespace) {
    EXPECT_EQ(trim(" \t hello world \n"), "hello world");
}

TEST(StringUtilsTest, TrimInPlaceUpdatesOriginalString) {
    std::string value = "  alpha beta  ";

    trim_in_place(value);

    EXPECT_EQ(value, "alpha beta");
}

TEST(StringUtilsTest, CaseConversionPreservesCharactersOutsideAlphabet) {
    EXPECT_EQ(to_lower("HeLLo-42"), "hello-42");
    EXPECT_EQ(to_upper("HeLLo-42"), "HELLO-42");
}

TEST(StringUtilsTest, SplitSupportsMultiCharacterAndCStringDelimiters) {
    const auto parts = split("a<>b<>c", "<>");
    const auto csv = split("red,green,blue", ",");

    EXPECT_EQ(parts, (std::vector<std::string>{"a", "b", "c"}));
    EXPECT_EQ(csv, (std::vector<std::string>{"red", "green", "blue"}));
}

TEST(StringUtilsTest, StartsWithAndEndsWithMatchExpectedPrefixes) {
    EXPECT_TRUE(starts_with("portpolio-lib", "port"));
    EXPECT_TRUE(ends_with("portpolio-lib", "lib"));
    EXPECT_FALSE(starts_with("portpolio-lib", "Lib"));
    EXPECT_FALSE(ends_with("portpolio-lib", "port"));
}

TEST(StringUtilsTest, JoinSupportsRangesAndVariadicArguments) {
    const std::array<std::string_view, 3> values{"one", "two", "three"};

    EXPECT_EQ(join(values, "/"), "one/two/three");
    EXPECT_EQ(join("/", "one", std::string("two"), std::string_view("three")),
              "one/two/three");
}

}  // namespace
}  // namespace psh::lib::utils
