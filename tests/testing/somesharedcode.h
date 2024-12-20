#pragma once

#ifndef Unit_Type
#define Unit_Type
struct Unit{};
#endif

//****************************************
#include <coroutine>
struct co_gethandle
{
    std::coroutine_handle<> _handle;

    bool await_ready() const noexcept { return false; }
    bool await_suspend(std::coroutine_handle<> handle) noexcept { _handle = handle; return false; }
    std::coroutine_handle<> await_resume() noexcept { return _handle; }
};

//****************************************
#include <hpp/expected>
using tl::expected;
using tl::unexpected;
using expected_ue = expected<Unit,std::exception_ptr>;


template <typename... Args>
struct std::coroutine_traits<expected_ue, Args...> {
    struct promise_type {
        auto get_return_object() { return std::true_type(); }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void(){}
        void unhandled_exception() {
            std::rethrow_exception(std::current_exception());
        }
    };
};

//****************************************
#include <chrono>
namespace cvk{
    inline uint64_t curTime(){return std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::system_clock::now().time_since_epoch()
                  ).count();}
}

//****************************************
#include <thread>
extern std::thread::id first;
extern std::thread::id second;

inline void checkAllThreadIDsCollision() {
    std::vector<std::thread::id> threadIDs = {
        first,
        second
    };

    for (size_t i = 0; i < threadIDs.size(); ++i) {
            for (size_t j = i + 1; j < threadIDs.size(); ++j) {
                if (threadIDs[j] not_eq std::thread::id()) {
                    assert(threadIDs[i] != threadIDs[j] && "Thread ID collision");
                }
            }
    }
}
inline void checkThread(std::thread::id& thread)
{
    if (thread == std::thread::id()) {
        thread = std::this_thread::get_id();
    } else {
        assert(thread == std::this_thread::get_id());
    }
    checkAllThreadIDsCollision();
}