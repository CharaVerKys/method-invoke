#pragma once

#include <cvk.hpp>

class Context3 : public cvk::Receiver{
    std::stop_token stop_token;
    
public:
    cvk::expected_contextsReg onAsyncStart(std::vector<std::function<void(std::stop_token)>>&& previousFuncs);
    
private:
    void asyncStart(std::stop_token);
    void firstInvoke(std::any any_ptr_promise_unit);
    void justAsync(std::any function_void_void);

    expected_ue traitedCoroutine();

    cvk::future<Unit> coroutine1(const char* text);
    cvk::future<int*> coroutine2();
};