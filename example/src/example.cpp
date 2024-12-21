// ! invoke method direct
// ! invoke method justSendAsync
// ! invoke method as coroutine await
// ! future wait
// ! future subscribe
// ! func that return future as coroutine

/*
context 1 create
context 2 create
context 3 create

contexts set receivers
contexts set start func (at least for getting stop token)
contexts run

context 1 invoke method from context 3 
context 3 invoke just async on self 
context 3 create coroutine work that->
    1. call invoke method on context 2
    2. call coroutine inside coroutine with native suspending
context 2 in subscribe request stop

exit all

then just all examples with *synchronic* work 

*/

#include <cvk.hpp>

#include <context1.hpp>
#include <context2.hpp>
#include <context3.hpp>

#include <syncwork.hpp>

int main(){
    Context1* context1 = new Context1; // works with ptr
    Context2 context2; // and objects itself
    Context3 context3;

    cvk::ContextsStore<3>* p_contextsStore = cvk::ContextsStore<3>::instance();
    #define and_then_async(e) and_then(e)
    
    try{ // global start pipe catch
        p_contextsStore->registerContexts(context1,&context2,&context3) // ! in order of enums
        .and_then(call_(context1, onAsyncStart))
        .and_then(call_(context2, onAsyncStart))
        .and_then(call_(context3, onAsyncStart))
            .or_else([](auto&&) -> std::invoke_result_t<decltype(&cvk::ContextsStore<0>::template registerContexts<cvk::Receiver>), cvk::ContextsStore<0>*, cvk::Receiver*>
            {
                throw std::runtime_error("failed to set async funcs");
            })
        .and_then(call_(p_contextsStore,startContexts))
            .or_else([](std::exception_ptr&&)->expected_ue{
                throw std::runtime_error("failed to start contexts");
            })
        // * using coroutine traits to complite with async work
        .and_then_async(call_(context1,startAsync))
        .value().subscribe([](tl::expected<Unit,std::exception_ptr>&&exc){
            std::cout << "call async in context 2 after subscribe on future from chained expected startup" <<std::endl;
        }
        , *cvk::contexts::getContext(SecondContext), true) // ? do in context 2 with force async
        ;
    }catch(...){
        std::cerr << "error";
        std::exit(1);
    }

    p_contextsStore->wait();

    syncWork();

    return 0;
}