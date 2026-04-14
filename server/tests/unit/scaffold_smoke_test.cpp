#include "types/not_null.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/thread_pool.hpp>

#include <atomic>

#include <gtest/gtest.h>

TEST(LibIntegrationTest, IncludesLibHeaderThroughServerTarget)
{
    auto value = psh::lib::types::NotNull<int>::Make(42);

    ASSERT_TRUE(static_cast<bool>(value.to_weak().lock()));
    EXPECT_EQ(*value, 42);
}
