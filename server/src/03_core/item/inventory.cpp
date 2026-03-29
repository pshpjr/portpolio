#include "03_core/item/inventory.h"
#include "03_core/item/item.h"

#include <cassert>

namespace psh::core {

bool Inventory::Insert(EntityId entityId, const NotNull<Item>& item)
{
    const auto  contentId = item->GetContentId();
    auto        datum = std::make_unique<Datum>(item, contentId, repo2_[contentId].size());

    auto [it, inserted] = repo1_.emplace(entityId, std::move(datum));
    if (!inserted)
        return false;

    repo2_[contentId].push_back(it->second.get());

    return true;
}

void Inventory::Delete(const NotNull<Item>& item)
{
    Delete(item->GetEntityId());
}

void Inventory::Delete(EntityId entityId)
{
    const auto node = repo1_.extract(entityId);
    if (node.empty())
        return;

    DeleteFromRepo2(*node.mapped());
}

auto Inventory::GetCount(ContentId contentId) -> std::size_t
{
    const auto it = repo2_.find(contentId);
    if (it == repo2_.end())
    {
        return 0;
    }

    std::size_t count = 0;
    for (const Datum* datum : it->second)
    {
        assert(datum != nullptr);
        count += datum->item->GetCurCount();
    }

    return count;
}

auto Inventory::GetSlotCount(ContentId contentId) -> std::size_t
{
    const auto it = repo2_.find(contentId);
    if (it == repo2_.end())
        return 0;

    return it->second.size();
}

bool Inventory::Contains(ContentId contentId)
{
    const auto it = repo2_.find(contentId);
    return it != repo2_.end() && !it->second.empty();
}

bool Inventory::Contains(EntityId entityId)
{
    return repo1_.find(entityId) != repo1_.end();
}

void Inventory::DeleteFromRepo2(Datum& datum)
{
    const auto bucketIt = repo2_.find(datum.contentId);
    assert(bucketIt != repo2_.end());

    auto& bucket = bucketIt->second;

    const auto slotIndex = datum.slotIndex;
    assert(slotIndex < bucket.size());
    assert(bucket[slotIndex] == &datum);

    if (&datum != bucket.back())
    {
        bucket[slotIndex] = bucket.back();
        bucket[slotIndex]->slotIndex = slotIndex;
    }

    bucket.pop_back();

    if (bucket.empty())
        repo2_.erase(bucketIt);
}

}  // namespace psh::core