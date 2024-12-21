#pragma once
#include "future.h"
#include <map>

namespace cvk{
class Receiver{
public:
    void private_invokeMethod(std::string_view method, std::unique_ptr<std::any>&& value);
    template<cvk::FutureValue T> // ! template
    void private_invokeMethod(std::string_view method, std::unique_ptr<std::any>&& value, cvk::promise<T>);

    template<class cl> requires std::is_base_of<Receiver, cl>::value
    void registerMethod(std::string_view s_method, void(cl::*p_method)(std::any)){
        methods_[s_method] = static_cast<void(Receiver::*)(std::any)>(p_method);
    }
    asio::io_context* private_getLoop(){return&loop_;}

protected:
    asio::io_context loop_;

private:
    std::map<std::string_view, void(Receiver::*)(std::any)> methods_; 
};
}//? namespace cvk