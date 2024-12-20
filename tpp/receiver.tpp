#pragma once
#include "../hpp/receiver.h"
#include "../hpp/methodinvoker.h"
#include <iostream>

template<cvk::FutureValue T>
void cvk::Receiver::private_invokeMethod(std::string_view method, std::unique_ptr<std::any>&& value,cvk::promise<T> prom){
    assert(value->has_value());
    assert(not method.empty());
    // cppcheck-suppress [assertWithSideEffect,incorrectStringBooleanError]
    assert(methods_.contains(method)&&"no suck method in this target" or [method](){std::cerr << method<<'\n';return false;}());
    asio::post(loop_,[this, method, value = std::move(*value), prom = std::move(prom)]() mutable{
        method::args<T> args{&prom,std::move(value)};

        (this->*methods_.at(method))
                        (std::make_any<method::args<T>>(args));

        bool setted = false;
        try{
            prom.set_exception({});
        }catch(...){ // value already setted
            setted = true;
        }
        assert(setted);
    });
}