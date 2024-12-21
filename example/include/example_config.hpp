#pragma once
#include <string_view>
enum ContextTargets : int16_t{ // ! in order of moveToContext
    InvalidContext = -1,
    FirstContext,
    SecondContext,
    ThirdContext
};
static_assert(FirstContext == 0);
static_assert(SecondContext == 1);
static_assert(ThirdContext == 2);
#define NUM_OF_CONTEXTS 3

//****************************************
using namespace std::string_view_literals;
namespace method{
    static inline constinit std::string_view justAsync = "justAsync"sv;
    static inline constinit std::string_view firstInvoke = "firstInvoke"sv;
    static inline constinit std::string_view secondInvoke = "secondInvoke"sv;
    static inline constinit std::string_view request_stop = "request_stop"sv;
}