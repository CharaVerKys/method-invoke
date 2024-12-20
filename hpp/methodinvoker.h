#pragma once 
#include "defines.h"
#include "future.h"

namespace cvk{
namespace method{
    void invoke(ContextTargets context, std::string_view method, std::any val = std::make_any<Unit>());
    template <cvk::FutureValue T> // ! template
    struct Invoke{
        Invoke(ContextTargets sender, ContextTargets receiver, std::string_view method, std::any args = std::make_any<Unit>());
        bool await_ready(){return false;}
        void await_suspend(std::coroutine_handle<>);
        T await_resume(){
            if(result.has_value()){
                return std::move(result.value());
            }
            std::rethrow_exception(result.error());
        }
    private:
        ContextTargets sender; ContextTargets receiver;
        std::string_view method; std::any args;
        tl::expected<T,std::exception_ptr> result;
        promise<T> promise;
        future<T> future;
    };

    template<cvk::FutureValue T>
    struct args{
        promise<T> *promise;
        std::any other;
    };
}
}//? namespace cvk