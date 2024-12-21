#include <context3.hpp>
#include <source_location>
#include <coroutine_traits.hpp>

cvk::expected_contextsReg Context3::onAsyncStart(std::vector<std::function<void(std::stop_token)>>&& previousFuncs){
    std::cout << std::source_location().function_name() <<'\n';
    
    regMethod(this,&Context3::firstInvoke);
    regMethod(this,&Context3::justAsync);

    assert(previousFuncs.size() == ThirdContext); // ? check that order is correct
    std::function<void(std::stop_token)> func = std::bind_front(&Context3::asyncStart, this);
    previousFuncs.push_back(std::move(func)); // ! push back only
    return previousFuncs;
}

void Context3::asyncStart(std::stop_token token){
    std::cout << std::source_location().function_name() <<'\n';
    stop_token = token;
}

void Context3::firstInvoke(std::any any_ptr_promise_unit){
    std::cout << std::source_location().function_name() <<'\n';
    std::shared_ptr<cvk::promise<Unit>> promise = std::any_cast<std::shared_ptr<cvk::promise<Unit>>>(std::move(any_ptr_promise_unit));
    
    traitedCoroutine();

    std::function<void()> func = [promise](){
        std::cout << "call just async lambda resume promise" << std::endl;
        promise->set_value({});
    };
    cvk::method::invoke(ThirdContext, method::justAsync,func);
    std::cout << "complete first invoke"<<std::endl;
}

void Context3::justAsync(std::any function_void_void){
    std::cout << std::source_location().function_name() <<'\n';
    std::any_cast<std::function<void()>>(std::move(function_void_void))();
}

expected_ue Context3::traitedCoroutine(){
    int8_t i = 21;
    const int result = co_await cvk::method::Invoke<int>{
        // ? from context to context
        // ? first is where coroutine will resume, second where func will calls
        ThirdContext, SecondContext,
        method::secondInvoke, i
    };
    assert(result == 42);

    // ? if you using single io_context you can overload operator co_await to use one global io context
    // ? but here is multiple io contexts and because of that awaiter not overloaded on future operation, but explicit construct
    Unit unit = co_await cvk::FutureAwaiter<Unit>{
        // cvk::contexts::getContext(ThirdContext),
        loop_,
        &Context3::coroutine1, this, "works"
    };
    std::cout << "coro chain complete"<<std::endl;
    cvk::method::invoke(SecondContext, method::request_stop);
    co_return;
}

cvk::future<Unit> Context3::coroutine1(const char* text){
    auto coro3 = []()->cvk::future<std::string>{
        co_return {"and this works"};
    };
    std::cout << text << std::endl;

    std::string text2 = co_await cvk::FutureAwaiter<std::string>{
        // cvk::contexts::getContext(ThirdContext),
        loop_,
        coro3
    };
    std::cout << text2 << std::endl;
    
    int* ptr = co_await cvk::FutureAwaiter<int*>{
        // cvk::contexts::getContext(ThirdContext),
        loop_,
        &Context3::coroutine2, this
    };
    delete[] ptr;

    co_return {};
}

cvk::future<int*> Context3::coroutine2(){
    co_return new int[4];
}