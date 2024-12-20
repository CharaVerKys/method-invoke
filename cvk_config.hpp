#pragma once
#include <string_view>
enum ContextTargets : int16_t{ // ! in order of moveToContext
    InvalidContext = -1,
    FirstContext,
    SecondContext
};
static_assert(FirstContext == 0);
static_assert(SecondContext == 1);

//****************************************
using namespace std::string_view_literals;
namespace method{
    static inline constinit std::string_view justSendAsync = "justSendAsync"sv;
    static inline constinit std::string_view getSettings = "getSettings"sv;
}