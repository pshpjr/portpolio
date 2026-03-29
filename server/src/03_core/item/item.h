//
// Created by pshpjr on 26. 3. 28..
//

#ifndef PORTPOLIO_ITEM_H
#define PORTPOLIO_ITEM_H
#include "01_types/content_id.h"
#include "01_types/entity_id.h"

namespace psh::core
{

class Item : public ContentId, public EntityId
{
public:
    auto GetCurCount() const -> std::size_t {return 0;}
    auto AddCount(size_t count) -> void;
private:
    size_t curCount_{};
    size_t maxCount_{};
    size_t oldCount_{};
};
}


#endif //PORTPOLIO_ITEM_H
