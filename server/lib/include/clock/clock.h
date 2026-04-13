#pragma once
#include <cstdint>

namespace psh::lib
{

class IClock
{
public:
    virtual ~IClock() = default;

    virtual uint64_t GetCurrentTime() const = 0;
    virtual uint64_t GetDeltaTime() const = 0;
    virtual uint64_t GetIdleTime() const = 0;
};

}  // namespace psh::lib
