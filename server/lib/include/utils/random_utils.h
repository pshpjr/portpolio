#pragma once

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <random>
#include <ranges>
#include <utility>

namespace psh::lib::utils
{

class RandomEngine;

namespace detail
{

[[nodiscard]] inline std::uint64_t MakeSeed()
{
    std::random_device device;
    const auto high = static_cast<std::uint64_t>(device());
    const auto low = static_cast<std::uint64_t>(device());

    return (high << 32U) ^ low;
}

inline RandomEngine& global_random_engine();

} // namespace detail

struct IntRange
{
    int min;
    int max;
};

struct FloatRange
{
    double min;
    double max;
};

class RandomEngine
{
public:
    RandomEngine()
        : m_engine(detail::MakeSeed())
    {
    }

    explicit RandomEngine(const std::uint64_t seed)
        : m_engine(seed)
    {
    }

    [[nodiscard]] int rand_int(int min, int max)
    {
        if (min > max)
        {
            std::swap(min, max);
        }

        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(m_engine);
    }

    [[nodiscard]] int rand_int(const IntRange range)
    {
        return rand_int(range.min, range.max);
    }

    [[nodiscard]] double rand_float(double min, double max)
    {
        if (min > max)
        {
            std::swap(min, max);
        }

        std::uniform_real_distribution<double> distribution(min, max);
        return distribution(m_engine);
    }

    [[nodiscard]] double rand_float(const FloatRange range)
    {
        return rand_float(range.min, range.max);
    }

    [[nodiscard]] bool rand_bool(double probability)
    {
        probability = std::clamp(probability, 0.0, 1.0);
        std::bernoulli_distribution distribution(probability);
        return distribution(m_engine);
    }

    template <std::ranges::random_access_range Container>
        requires std::ranges::sized_range<Container>
    [[nodiscard]] decltype(auto) rand_pick(Container& container)
    {
        const auto size = std::ranges::size(container);
        const auto index = static_cast<std::size_t>(rand_int(0, static_cast<int>(size - 1)));

        return container[index];
    }

    template <typename Container>
    void shuffle(Container& container)
    {
        std::shuffle(std::begin(container), std::end(container), m_engine);
    }

private:
    std::mt19937_64 m_engine;
};

namespace detail
{

inline RandomEngine& global_random_engine()
{
    thread_local RandomEngine engine;
    return engine;
}

} // namespace detail

[[nodiscard]] inline int Rand(const int min, const int max)
{
    return detail::global_random_engine().rand_int(min, max);
}

[[nodiscard]] inline int Rand(const IntRange range)
{
    return detail::global_random_engine().rand_int(range);
}

[[nodiscard]] inline double Rand(const double min, const double max)
{
    return detail::global_random_engine().rand_float(min, max);
}

[[nodiscard]] inline double Rand(const FloatRange range)
{
    return detail::global_random_engine().rand_float(range);
}

[[nodiscard]] inline bool Rand_bool(const double probability)
{
    return detail::global_random_engine().rand_bool(probability);
}

} // namespace psh::lib::utils
