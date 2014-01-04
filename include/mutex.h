/*=============================================================================
#     FileName: mutex
#         Desc: locks implementation for cpp class
#       Author: Jeffrey Pu
#        Email: pujunying@gmail.com
#     HomePage: https://github.com/jfpu
#      Version: 0.0.1
#   LastChange: 2014-01-04 17:18:34
#      History:
=============================================================================*/

#ifndef __JFPU_MUTEX_H_
#define __JFPU_MUTEX_H_

#include <cerrno>

#define SPIN_INTERVAL_RELAX 1000
// #define SPIN_INTERVAL_RELAX 0
#define barrier() __asm__ __volatile__("" : : : "memory")

namespace jfpu {

class spinlock {
    // volatile _Atomic_word _value;
    volatile int _value;

    spinlock(const spinlock& rhs);
    spinlock& operator=(const spinlock& rhs);

public:
    spinlock() {_value = 0;}
    ~spinlock() {_value = 0;}

    int lock() {
        // Atomicly get the lock
        // if(0 == __gnu_cxx::__atomic_add_dispatch(&_value, 1)
        // C++11 Atomic operations: #include <atomic>
        // template< class T >
        // bool atomic_compare_exchange_weak(std::atomic<T>* obj,
        //                                   T* expected,
        //                                   T desired );
        // template< class T >
        // bool atomic_compare_exchange_weak(volatile std::atomic<T>* obj,
        //                                   T* expected, T desired );
        if(__sync_bool_compare_and_swap(&_value, 0, 1)) {
            return 0;
        }

        // spin
        do {
            if(0 < SPIN_INTERVAL_RELAX) {
                int wait = SPIN_INTERVAL_RELAX;

                while(0 != _value && 0 < wait)
                    --wait;
            }
            else {
                while(0 != _value)
                    ;
            }
        } while(!__sync_bool_compare_and_swap(&_value, 0, 1));

        return 0;
    }

    int trylock() {
        return __sync_bool_compare_and_swap(&_value, 0, 1) ? 0 : EBUSY;
    }

    // #define barrier() __asm__ __volatile__("": : :"memory")
    int unlock() {
        barrier();
        _value = 0;
        return 0;
    }

};

// Do not acquire ownership of the mutex.
struct defer_lock_t { };

// Try to acquire ownership of the mutex without blocking.
struct try_to_lock_t { };

// Assume the calling thread has already obtained mutex ownership and manage it.
struct adopt_lock_t { };

constexpr defer_lock_t      defer_lock{};
constexpr try_to_lock_t     try_to_lock{};
constexpr adopt_lock_t      adopt_lock{};


template<typename _lock>
class lock_guard {
public:
    typedef _lock lock_type;
private:
    lock_type& _m;
public:
    explicit lock_guard(lock_type& m) : _m(m) {
        _m.lock();
    }

    // Calling thread owns mutex
    lock_guard(lock_type& m, adopt_lock_t) : _m(m) {}

    ~lock_guard() {
        _m.unlock();
    }

#if 0
private:
    lock_guard(const lock_guard& );
    lock_guard& operator=(const lock_guard& );
#endif
    lock_guard(const lock_guard& ) = delete;
    lock_guard& operator=(const lock_guard& ) = delete;
};


}

#endif
