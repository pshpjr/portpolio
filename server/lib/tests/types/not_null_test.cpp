#include "types/not_null.h"

#include <memory>
#include <string>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

namespace psh::lib::types
{
namespace
{

struct TestValue
{
    explicit TestValue(int initialValue, std::string initialName = {})
        : value(initialValue), name(std::move(initialName))
    {
    }

    void increment()
    {
        ++value;
    }

    [[nodiscard]] int doubled() const
    {
        return value * 2;
    }

    int value;
    std::string name;
};

TEST(NotNullTest, MakeCreatesNonNullSharedObject)
{
    auto value = NotNull<TestValue>::Make(7, "alpha");

    EXPECT_EQ(value->value, 7);
    EXPECT_EQ(value->name, "alpha");
}

TEST(NotNullTest, FromRejectsNullSharedPtr)
{
    std::shared_ptr<TestValue> ptr;

    auto value = NotNull<TestValue>::From(std::move(ptr));

    EXPECT_FALSE(value.has_value());
}

TEST(NotNullTest, FromConsumesSharedPtrAndPreservesOwnership)
{
    auto ptr = std::make_shared<TestValue>(11);
    auto raw = ptr.get();

    auto value = NotNull<TestValue>::From(std::move(ptr));

    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(ptr, nullptr);
    EXPECT_EQ(value->operator->(), raw);
}

TEST(NotNullTest, CopyAndMoveOperationsKeepInvariant)
{
    auto original = NotNull<TestValue>::Make(3);
    auto copy = original;
    auto moved = std::move(original);

    copy->increment();

    EXPECT_EQ(moved->value, 4);
}

TEST(NotNullTest, DereferenceOperatorsExposeUnderlyingObject)
{
    auto value = NotNull<TestValue>::Make(5);

    (*value).increment();

    EXPECT_EQ(value->value, 6);
    EXPECT_EQ((*value).doubled(), 12);
}

TEST(NotNullTest, ImplicitSharedPtrConversionReturnsSharedOwnership)
{
    auto value = NotNull<TestValue>::Make(9);

    std::shared_ptr<TestValue> shared = value;

    ASSERT_NE(shared, nullptr);
    EXPECT_EQ(shared->value, 9);
    EXPECT_EQ(shared.use_count(), 2);
}

TEST(NotNullTest, ToWeakReturnsLockableWeakPtr)
{
    auto value = NotNull<TestValue>::Make(13);

    auto weak = value.to_weak();
    auto locked = weak.lock();

    ASSERT_NE(locked, nullptr);
    EXPECT_EQ(locked->value, 13);
}

static_assert(!std::is_default_constructible_v<NotNull<TestValue>>);
static_assert(std::is_copy_constructible_v<NotNull<TestValue>>);
static_assert(std::is_move_constructible_v<NotNull<TestValue>>);

} // namespace
} // namespace psh::lib::types
