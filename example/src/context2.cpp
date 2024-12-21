#include <context2.hpp>
#include <source_location>

cvk::expected_contextsReg Context2::onAsyncStart(std::vector<std::function<void(std::stop_token)>>&& previousFuncs){
    std::cout << std::source_location().function_name() <<'\n';

    regMethod(this,&Context2::secondInvoke);
    regMethod(this,&Context2::request_stop);

    assert(previousFuncs.size() == SecondContext); // ? check that order is correct
    std::function<void(std::stop_token)> func = std::bind_front(&Context2::asyncStart, this);
    previousFuncs.push_back(std::move(func)); // ! push back only
    return previousFuncs;
}


void Context2::asyncStart(std::stop_token token){
    std::cout << std::source_location().function_name() <<'\n';
    stop_token = token;
}

void Context2::secondInvoke(std::any any){
    // ? method::args<promise_value>
    auto [promise,args] = std::any_cast<cvk::method::args<int>>(std::move(any));
    int8_t i = std::any_cast<int8_t>(std::move(args));
    promise->set_value(i*2);
}

void Context2::request_stop(std::any){
    cvk::ContextsStore<3>::instance()->requestStop();
}