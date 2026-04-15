//
// Created by pshpjr on 26. 3. 28..
//

#include "item.h"
auto psh::core::Item::AddCount(size_t count) -> void
{
    if (count == 0)
        return;

    auto newCount =
        count > 0 ? std::max(curCount_ + count, maxCount_) : std::min(curCount_ + count, 0ull);
    oldCount_ = std::exchange(curCount_, newCount);
}