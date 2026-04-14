#pragma once

#include "common/content_id.h"
#include "common/entity_id.h"
#include "common/type_def.h"
#include "types/not_null.h"

#include <memory>
#include <utility>

namespace psh::core
{

class Item;

using namespace lib::types;

class Inventory
{
    struct Datum
    {
        explicit Datum(NotNull<Item> item, ContentId contentId, size_t slotIndex)
            : item(std::move(item)),
              contentId(contentId),
              slotIndex(slotIndex)
        {
        }

        NotNull<Item> item;
        ContentId contentId{};
        size_t slotIndex{};
    };

public:
    auto begin()
    {
        return repo1_.begin();
    } // NOLINT(*-identifier-naming)
    auto end()
    {
        return repo1_.end();
    } // NOLINT(*-identifier-naming)

public:
    bool Insert(EntityId entityId, const NotNull<Item>& item);
    void Delete(const NotNull<Item>& item);
    void Delete(EntityId entityId);

    auto GetCount(ContentId contentId) -> std::size_t;
    auto GetSlotCount(ContentId contentId) -> std::size_t;

    bool Contains(ContentId contentId);
    bool Contains(EntityId entityId);

private:
    void DeleteFromRepo2(Datum& datum);

    //  멀티 키 인덱스
    //  주소 변경 이슈 고려 unique로 설정
    HashMap<EntityId, std::unique_ptr<Datum>> repo1_; //  아이템 원본
    HashMap<ContentId, Vector<Datum*>> repo2_;        //  해당 타입 아이템 인덱스
};

} // namespace psh::core
