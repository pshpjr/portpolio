#pragma once

/**
 * @brief CRTP 기반 싱글턴 헬퍼
 *
 * 사용법:
 *   class MyManager : public Singleton<MyManager> {
 *       friend class Singleton<MyManager>;
 *   private:
 *       MyManager() = default;
 *   };
 *
 *   MyManager::get().doSomething();
 */
template <typename T>
class Singleton {
public:
    static T& get() {
        static T instance{};
        return instance;
    }

    Singleton(const Singleton&)            = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&)                 = delete;
    Singleton& operator=(Singleton&&)      = delete;

protected:
    Singleton()  = default;
    ~Singleton() = default;
};
