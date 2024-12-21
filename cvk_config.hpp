#pragma once
#include <cstdint>
enum ContextTargets : int16_t{ // ! in order of moveToContext
    InvalidContext = -1,
    FirstContext,
    SecondContext
};
static_assert(FirstContext == 0);
static_assert(SecondContext == 1);
// * this define required to work properly
#define NUM_OF_CONTEXTS 2
// ? you also can define something like 
// #define ContStore ContextsStore<2>

//****************************************
// #include <string_view>
// using namespace std::string_view_literals;
// namespace method{
    // static inline constinit std::string_view func1 = "func1"sv;
    // static inline constinit std::string_view func2 = "func2"sv;
// }