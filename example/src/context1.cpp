#include <context1.hpp>
#include <source_location>

cvk::expected_contextsReg Context1::onAsyncStart(std::vector<std::function<void(std::stop_token)>>&& previousFuncs){
    std::cout << std::source_location().function_name() <<'\n';
    assert(previousFuncs.size() == FirstContext); // ? check that order is correct
    std::function<void(std::stop_token)> func = std::bind_front(&Context1::asyncStart, this);
    previousFuncs.push_back(std::move(func)); // ! push back only
    return previousFuncs;
}

tl::expected<cvk::future<Unit>,std::exception_ptr> Context1::startAsync(Unit&&){
    std::cout << std::source_location().function_name() <<'\n';
    
    // ? main thread now
    
    cvk::promise<Unit> promise;
    cvk::future<Unit> future = promise.get_future();

    std::shared_ptr<cvk::promise<Unit>> ptr_promise = std::make_shared<cvk::promise<Unit>>(std::move(promise));

    cvk::method::invoke(ThirdContext, method::firstInvoke, ptr_promise);

    
    return std::move(future);
}

void Context1::asyncStart(std::stop_token token){
    std::cout << std::source_location().function_name() <<'\n';
    stop_token = token;
}