#pragma once
#ifndef Unit_Type
#define Unit_Type
struct Unit{};
#endif

#ifndef __cvk_config
#define __cvk_config <cvk_config.hpp>
#endif
#include __cvk_config

namespace cvk {
consteval const char* extract_method_name(const char* fullString) { // обязательно compile-time
    const char* literPos = nullptr;
    for (const char* itPos = fullString; *itPos; ++itPos) {
        if(*itPos == 0){throw "wrong format";}
        if (*itPos == ':') { // последнее совпадение найдётся
            literPos = itPos;
        }
    }
    return literPos ? literPos + 1 : fullString;  // если подан по какой-то причине только метод
}
}//? namespace cvk

#define regMethod(res, method) res->registerMethod(cvk::extract_method_name(#method),method);
#define call_(obj, method) \
    [&obj](auto&& expect) { \
        return std::invoke(&std::remove_pointer_t<std::remove_reference_t<decltype(obj)>>::method, obj, std::forward<decltype(expect)>(expect)); \
    }
#define call_this(method) \
    [this](auto&& expect) { \
        return std::invoke(&std::remove_pointer_t<decltype(this)>::method, this, std::forward<decltype(expect)>(expect)); \
    }

#include "expected"
#include <exception>
#include <stop_token>
using expected_ue = tl::expected<Unit,std::exception_ptr>;
namespace cvk {
using expected_contextsReg = tl::expected<std::vector<std::function<void(std::stop_token)>>,std::exception_ptr>;
}// ? namespace cvk