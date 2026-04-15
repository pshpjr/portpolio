//
// Created by admin on 26. 4. 15..
//

#ifndef PORTPOLIO_ATOMIC_UTIL_H
#define PORTPOLIO_ATOMIC_UTIL_H
#include <atomic>
namespace psh::lib::utils
{

template <class T>
T FetchMax(std::atomic<T>& a, T arg, std::memory_order order = std::memory_order_seq_cst)
{
    T old = a.load(std::memory_order_relaxed);
    while (old < arg && !a.compare_exchange_weak(old, arg, order, std::memory_order_relaxed))
    {
    }

    return old;
}

}
#endif //PORTPOLIO_ATOMIC_UTIL_H
