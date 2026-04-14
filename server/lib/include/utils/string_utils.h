#pragma once

#include <algorithm>
#include <array>
#include <cctype>
#include <concepts>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace psh::lib::utils
{
namespace detail
{

[[nodiscard]] inline bool IsSpace(const char ch) noexcept
{
    return std::isspace(static_cast<unsigned char>(ch)) != 0;
}

[[nodiscard]] inline std::string_view TrimView(const std::string_view value) noexcept
{
    size_t start = 0;
    size_t end = value.size();

    while (start < end && IsSpace(value[start]))
    {
        ++start;
    }

    while (end > start && IsSpace(value[end - 1]))
    {
        --end;
    }

    return value.substr(start, end - start);
}

template <typename T>
[[nodiscard]] inline std::string_view ToStringView(const T& value)
{
    if constexpr (std::is_same_v<std::remove_cvref_t<T>, std::string>)
    {
        return std::string_view(value);
    }
    else if constexpr (std::is_same_v<std::remove_cvref_t<T>, std::string_view>)
    {
        return value;
    }
    else
    {
        return std::string_view(value);
    }
}

} // namespace detail

[[nodiscard]] inline std::string trim(const std::string_view value)
{
    return std::string(detail::TrimView(value));
}

inline void trim_in_place(std::string& value)
{
    value = trim(value);
}

[[nodiscard]] inline std::string to_lower(const std::string_view value)
{
    std::string result(value);

    std::transform(result.begin(), result.end(), result.begin(), [](const unsigned char ch)
                   {
                       return static_cast<char>(std::tolower(ch));
                   });

    return result;
}

[[nodiscard]] inline std::string to_upper(const std::string_view value)
{
    std::string result(value);

    std::transform(result.begin(), result.end(), result.begin(), [](const unsigned char ch)
                   {
                       return static_cast<char>(std::toupper(ch));
                   });

    return result;
}

[[nodiscard]] inline std::vector<std::string> split(const std::string_view src,
                                                    const std::string_view delim)
{
    if (delim.empty())
    {
        return {std::string(src)};
    }

    std::vector<std::string> parts;
    size_t start = 0;

    while (start <= src.size())
    {
        const size_t pos = src.find(delim, start);

        if (pos == std::string_view::npos)
        {
            parts.emplace_back(src.substr(start));
            break;
        }

        parts.emplace_back(src.substr(start, pos - start));
        start = pos + delim.size();
    }

    return parts;
}

[[nodiscard]] inline std::vector<std::string> split(const std::string_view src, const char* delim)
{
    return split(src, std::string_view(delim));
}

template <size_t N>
[[nodiscard]] inline std::vector<std::string> split(const std::string_view src,
                                                    const char (&delim)[N])
{
    return split(src, std::string_view(delim, N - 1));
}

[[nodiscard]] inline constexpr bool starts_with(const std::string_view value,
                                                const std::string_view prefix) noexcept
{
    return value.size() >= prefix.size() && value.compare(0, prefix.size(), prefix) == 0;
}

[[nodiscard]] inline constexpr bool ends_with(const std::string_view value,
                                              const std::string_view suffix) noexcept
{
    return value.size() >= suffix.size() &&
           value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
}

template <std::ranges::input_range Range>
[[nodiscard]] inline std::string join(Range&& range, const std::string_view delim)
{
    std::string result;
    bool isFirst = true;

    for (auto&& item : range)
    {
        if (!isFirst)
        {
            result.append(delim);
        }

        result.append(detail::ToStringView(item));
        isFirst = false;
    }

    return result;
}

template <typename... Args>
    requires(sizeof...(Args) > 0)
[[nodiscard]] inline std::string join(const std::string_view delim, Args&&... args)
{
    return join(std::array<std::string_view, sizeof...(Args)>{detail::ToStringView(
                    std::forward<Args>(args))...},
                delim);
}

} // namespace psh::lib::utils
