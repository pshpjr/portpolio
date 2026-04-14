#pragma once

#include <algorithm>
#include <functional>
#include <ranges>
#include <type_traits>
#include <utility>
#include <vector>

namespace psh::lib::utils
{

template <std::ranges::input_range Range, typename T>
[[nodiscard]] constexpr bool contains(Range&& range, const T& value)
{
    return std::ranges::find(range, value) != std::ranges::end(range);
}

template <std::ranges::input_range Range, typename Predicate, typename DefaultValue>
[[nodiscard]] constexpr auto find_if_or(Range&& range, Predicate predicate,
                                        DefaultValue&& defaultValue)
    -> std::ranges::range_value_t<Range>
{
    auto it = std::ranges::find_if(range, std::move(predicate));

    if (it != std::ranges::end(range))
    {
        return *it;
    }

    return static_cast<std::ranges::range_value_t<Range>>(std::forward<DefaultValue>(defaultValue));
}

template <typename Container, typename Predicate>
constexpr auto erase_if(Container& container, Predicate predicate)
{
    return std::erase_if(container, std::move(predicate));
}

template <std::ranges::input_range Range, typename Function>
[[nodiscard]] constexpr auto transform_to_vec(Range&& range, Function function)
{
    using ResultType =
        std::remove_cvref_t<std::invoke_result_t<Function&, std::ranges::range_reference_t<Range>>>;

    std::vector<ResultType> result;

    for (auto&& value : range)
    {
        result.emplace_back(std::invoke(function, value));
    }

    return result;
}

template <std::ranges::viewable_range Range>
[[nodiscard]] constexpr auto keys(Range&& range)
{
    return std::forward<Range>(range) | std::views::keys;
}

template <std::ranges::viewable_range Range>
[[nodiscard]] constexpr auto values(Range&& range)
{
    return std::forward<Range>(range) | std::views::values;
}

} // namespace psh::lib::utils
