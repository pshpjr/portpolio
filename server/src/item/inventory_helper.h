//
// Created by pshpjr on 26. 3. 29..
//

#ifndef PORTPOLIO_INVENTORY_HELPER_H
#define PORTPOLIO_INVENTORY_HELPER_H
#include "item.h"

namespace psh::core
{
class inventory_helper {

    bool CanAdd(const Item& item, size_t count);
    bool CanRemove(const Item& item, size_t count);
};
}




#endif //PORTPOLIO_INVENTORY_HELPER_H
