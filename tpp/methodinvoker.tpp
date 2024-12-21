#pragma once
#include "../hpp/methodinvoker.h"
#include "../hpp/iocontextgetter.h"
#include "receiver.tpp"

template <cvk::FutureValue T>
void cvk::method::Invoke<T>::await_suspend(std::coroutine_handle<>cont){
    const asio::io_context* sender_ = contexts::getContext(sender);
    future.subscribe([this,cont](tl::expected<T,std::exception_ptr>&& expected){
        result = std::move(expected);
        assert(cont);
        assert(not cont.done());
        cont();
    }, *sender_);

    const Receiver* receiver_ = contexts::getReceiver(receiver);
    assert(receiver);
    std::unique_ptr<std::any> p_val = std::make_unique<std::any>(std::move(args));
    const_cast<Receiver*>(receiver_)
        ->private_invokeMethod<T>(method, std::move(p_val),std::move(promise));
}

template <cvk::FutureValue T>
cvk::method::Invoke<T>::Invoke(ContextTargets sender, ContextTargets receiver, std::string_view method, std::any args)
:sender(sender), receiver(receiver),method(method),args(args),promise(),future(promise.get_future())
{}