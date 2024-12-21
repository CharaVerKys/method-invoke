#pragma once

#include <cvk.hpp>
#include <coroutine_traits.hpp>

class Context1 : public cvk::Receiver{
    std::stop_token stop_token;
    
public:
    cvk::expected_contextsReg onAsyncStart(std::vector<std::function<void(std::stop_token)>>&& previousFuncs);
    tl::expected<cvk::future<Unit>,std::exception_ptr> startAsync(Unit&&);
    
private:
    void asyncStart(std::stop_token);
};