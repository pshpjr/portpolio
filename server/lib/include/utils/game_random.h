#pragma once

#include "types/type_def.h"
#include "utils/random_utils.h"

#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <vector>

namespace psh::lib::utils
{

struct BagDrawState
{
    UInt64 seed;
    int bag_size;
    int index;
};

template <typename T>
class BagDraw
{
public:
    explicit BagDraw(std::initializer_list<std::pair<T, int>> weightedItems)
        : m_weightedItems(weightedItems),
          m_seed(detail::MakeSeed()),
          m_engine(m_seed)
    {
        refill_bag();
        m_engine.shuffle(m_bag);
    }

    BagDraw(std::initializer_list<std::pair<T, int>> weightedItems, const BagDrawState& savedState)
        : m_weightedItems(weightedItems),
          m_seed(savedState.seed),
          m_index(savedState.index),
          m_engine(m_seed)
    {
        refill_bag();

        if (savedState.bag_size != static_cast<int>(m_bag.size()))
        {
            throw std::invalid_argument("saved bag_size does not match weights");
        }

        if (savedState.index < 0 || savedState.index >= savedState.bag_size)
        {
            throw std::out_of_range("saved index is outside bag bounds");
        }

        m_engine.shuffle(m_bag);
    }

    [[nodiscard]] const T& draw()
    {
        if (m_index >= static_cast<int>(m_bag.size()))
        {
            start_new_cycle();
        }

        return m_bag[static_cast<std::size_t>(m_index++)];
    }

    [[nodiscard]] BagDrawState save_state() const
    {
        return BagDrawState{m_seed, static_cast<int>(m_bag.size()), m_index};
    }

    [[nodiscard]] int remaining() const
    {
        return static_cast<int>(m_bag.size()) - m_index;
    }

private:
    void refill_bag()
    {
        m_bag.clear();

        for (const auto& [value, weight] : m_weightedItems)
        {
            if (weight <= 0)
            {
                continue;
            }

            for (int count = 0; count < weight; ++count)
            {
                m_bag.push_back(value);
            }
        }

        if (m_bag.empty())
        {
            throw std::invalid_argument("BagDraw requires positive weights");
        }
    }

    void start_new_cycle()
    {
        m_seed = detail::MakeSeed();
        m_engine = RandomEngine(m_seed);
        refill_bag();
        m_engine.shuffle(m_bag);
        m_index = 0;
    }

    std::vector<std::pair<T, int>> m_weightedItems;
    std::vector<T> m_bag;
    UInt64 m_seed = 0;
    int m_index = 0;
    RandomEngine m_engine;
};

} // namespace psh::lib::utils
