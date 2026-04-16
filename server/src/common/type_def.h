//
// Created by pshpjr on 26. 3. 28..
//

#ifndef PORTPOLIO_TYPE_DEF_H
#define PORTPOLIO_TYPE_DEF_H
#include "types/strong_id.h"

#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

template <typename T>
using Vector = std::vector<T>;

template <typename T>
using Set = std::set<T>;

template <typename Key, typename Val>
using Map = std::map<Key, Val>;

template <typename T>
using HashSet = std::unordered_set<T>;

template <typename Key, typename Val>
using HashMap = std::unordered_map<Key, Val>;

using UserID = psh::lib::types::StrongId<UInt64>;
using AccountID = psh::lib::types::StrongId<UInt64>;
using ItemID = psh::lib::types::StrongId<UInt64>;
using SkillID = psh::lib::types::StrongId<UInt64>;

#endif // PORTPOLIO_TYPE_DEF_H
