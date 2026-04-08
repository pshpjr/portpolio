//
// Created by pshpjr on 26. 3. 28..
//

#ifndef PORTPOLIO_CONTENT_ID_H
#define PORTPOLIO_CONTENT_ID_H

#include "types/type_def.h"

#include <functional>

namespace psh
{
//  기획 테이블에 있는 ID
class ContentId
{
public:
    constexpr ContentId() = default;
    constexpr ContentId(UInt64 contentId) : contentId_(contentId) {}

    explicit operator UInt64() const { return contentId_; }

    [[nodiscard]] auto GetContentId() const -> UInt64 { return contentId_; }
    void SetContentId(const UInt64& contentId) { contentId_ = contentId;}

    constexpr ContentId(const ContentId&) = default;
    constexpr ContentId(ContentId&&) = default;

    ContentId& operator=(const ContentId&) = default;
    ContentId& operator=(ContentId&&) = default;

    constexpr bool operator==(const ContentId& other) const = default;
private:
    UInt64 contentId_{0};
};

}

template <>
struct std::hash<psh::ContentId> {
    std::size_t operator()(const psh::ContentId& contentId) const {
        return static_cast<std::size_t>(contentId);
    }
};


#endif //PORTPOLIO_CONTENT_ID_H
