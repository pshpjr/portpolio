#pragma once

#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

namespace portpolio::lib::types {

template <typename T>
class NotNull {
    static_assert(!std::is_array_v<T>,
                  "NotNull does not support array element types.");

public:
    using element_type = T;

    NotNull() = delete;
    NotNull(const NotNull&) = default;
    NotNull(NotNull&&) noexcept = default;
    NotNull& operator=(const NotNull&) = default;
    NotNull& operator=(NotNull&&) noexcept = default;
    ~NotNull() = default;

    template <typename... Args>
    static NotNull Make(Args&&... args) {
        return NotNull(std::make_shared<T>(std::forward<Args>(args)...));
    }

    static std::optional<NotNull> From(std::shared_ptr<T>&& ptr) noexcept {
        if (!ptr) {
            return std::nullopt;
        }

        return NotNull(std::move(ptr));
    }

    T* operator->() noexcept { return ptr_.get(); }

    const T* operator->() const noexcept { return ptr_.get(); }

    T& operator*() noexcept { return *ptr_; }

    const T& operator*() const noexcept { return *ptr_; }

    operator std::shared_ptr<T>() const noexcept { return ptr_; }

    std::weak_ptr<T> to_weak() const noexcept { return ptr_; }

private:
    explicit NotNull(std::shared_ptr<T>&& ptr) noexcept
        : ptr_(std::move(ptr)) {}

    std::shared_ptr<T> ptr_;
};

}  // namespace portpolio::lib::types
