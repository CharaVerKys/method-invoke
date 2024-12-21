#pragma once

#include <cvk.hpp>

template <typename... Args>
struct std::coroutine_traits<expected_ue, Args...> {
    struct promise_type {
        auto get_return_object() { return Unit(); }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void(){}
        void unhandled_exception() {
            std::rethrow_exception(std::current_exception());
        }
    };
};