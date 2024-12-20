#include "../hpp/receiver.h"
#include <iostream>
#include <asio/post.hpp>

inline void cvk::Receiver::private_invokeMethod(std::string_view method, std::unique_ptr<std::any>&& value){
    assert(value->has_value());
    assert(not method.empty());
    // cppcheck-suppress assertWithSideEffect
    // cppcheck-suppress incorrectStringBooleanError
    assert(methods_.contains(method)&&"no suck method in this target" or [method](){std::cerr << method<<'\n';return false;}());
    auto lambda =
        [this, method, value = std::move(*value)]() mutable{
        (this->*methods_.at(method))(std::move(value));
    };
    asio::post(loop_, lambda);
}