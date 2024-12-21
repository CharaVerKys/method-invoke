#pragma once

#include <cvk.hpp>

class Context2 : public cvk::Receiver{
    std::stop_token stop_token;

public:
    cvk::expected_contextsReg onAsyncStart(std::vector<std::function<void(std::stop_token)>>&& previousFuncs);
    
private:
    void asyncStart(std::stop_token);
    void secondInvoke(std::any any);
    void request_stop(std::any);
};