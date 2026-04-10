#pragma once

namespace psh::lib {

class IClock {
public:
    virtual ~IClock() = default;

    virtual double GetCurrentTime() const = 0;
    virtual double GetDeltaTime() const  = 0;
    virtual double GetIdleTime() const   = 0;
};

}  // namespace psh::lib
