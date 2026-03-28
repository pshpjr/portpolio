#pragma once

#include "01_types/content_id.h"
#include "01_types/entity_id.h"
#include "01_types/type_def.h"
#include "types/not_null.h"

#include <memory>

namespace psh::core {

class Item;

using namespace lib::types;

class Inventory
{
    struct Datum
    {
        NotNull<Item> item{};
        ContentId contentId{};
        size_t slotIndex{};
    };

public:
    auto begin() { return m_repo1.begin(); }
    auto end() { return m_repo1.end(); }

public:
    bool Insert(const NotNull<Item>& item);
    void Delete(const NotNull<Item>& item);
    void Delete(EntityId entityId);

    auto GetCount(ContentId contentId) -> std::size_t;
    auto GetSlotCount(ContentId contentId) -> std::size_t;

    bool Contains(ContentId contentId);
    bool Contains(EntityId entityId);

private:
    //  멀티 키 인덱스
    //  주소 변경 이슈 고려 unique로 설정
    HashMap<EntityId, std::unique_ptr<Datum>> m_repo1;  //  아이템 원본
    HashMap<ContentId, Vector<Datum*>> m_repo2; //  해당 타입 아이템 인덱스
};


}  // namespace psh::core
