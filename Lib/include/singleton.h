#pragma once

#include <cassert>
#include <type_traits>

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
 *   MyManager::instance().doSomething();
 */
template <typename T>
class Singleton {
public:
    static T& instance() {
        assert(s_instance != nullptr && "Singleton not initialized");
        return *s_instance;
    }

    static void create() {
        assert(s_instance == nullptr && "Singleton already created");
        static T obj{};
        s_instance = &obj;
    }

    static void destroy() {
        s_instance = nullptr;
    }

    static bool is_created() {
        return s_instance != nullptr;
    }

    Singleton(const Singleton&)            = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&)                 = delete;
    Singleton& operator=(Singleton&&)      = delete;

protected:
    Singleton() = default;
    ~Singleton() = default;

private:
    inline static T* s_instance = nullptr;
};
