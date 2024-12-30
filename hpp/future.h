#pragma once
#include <asio/io_context.hpp>
#include <asio/post.hpp>
#include <condition_variable>
#include "expected"
#include <mutex>
#include <stdexcept>

// ? beyond namespace for awaiter and coroutine
#include <coroutine>
namespace cvk{

template<typename T>
concept FutureValue
=   std::is_constructible<T>::value
and std::is_move_constructible_v<T>;

namespace details{
    template <FutureValue T>
    struct shared_state{
        bool valid_ = false;
        bool used_ = false;
        std::atomic_uint8_t alive_ = 1;
        std::function<void(tl::expected<T,std::exception_ptr>&&)> callback_ = nullptr;
        tl::expected<T,std::exception_ptr> expected_;
        const asio::io_context* context_ = nullptr;
        std::mutex mutex_;
        std::condition_variable cond_var_;
    };
}


template <FutureValue U>
class promise;

template <FutureValue T>
class future{
    template <FutureValue U>
    friend class cvk::promise;

    details::shared_state<T>* state_ = nullptr;
    // * sync with get_future
    explicit future(details::shared_state<T>* state):state_(state){state_->alive_.fetch_add(1,std::memory_order_relaxed);}

    future(future&) = delete; 
    void operator=(future&other) = delete;
public:
    future(future&&o):state_(o.state_){o.state_ = nullptr;}
    void operator=(future&&other){
        // //assert(not state);
        this->state_ = other.state_;
        other.state_ = nullptr;
    }
   
    // ! if future is ready will call callback immediately
    // ? use force async if you want to be sure op is async
    void subscribe(std::function<void(tl::expected<T,std::exception_ptr>&&)>callback, const asio::io_context& context, bool forceAsync = false){
        if(not state_){throw std::logic_error("use moved future");}
        std::unique_lock lock(state_->mutex_);
        if(state_->used_){throw std::logic_error("future use second time");}
        state_->used_ = true;
        if(state_->valid_){
            if(not forceAsync){
                callback(std::move(state_->expected_));
            }else{ // ? force async operation
                asio::post(*const_cast<asio::io_context*>(&context),
                [callback = std::move(state_->callback_),
                expected = std::move(state_->expected_)]() mutable{
                    callback(std::move(expected));
                });
            }return;
        }
        state_->context_ = &context;
        state_->callback_ = std::move(callback);
    }
    T get(){
        if(not state_){throw std::logic_error("use moved future");}
        std::unique_lock lock(state_->mutex_);
        if(state_->used_){throw std::logic_error("future use second time");}
        state_->used_ = true;
        state_->cond_var_.wait(lock,[this](){return state_->alive_.load(std::memory_order_relaxed) == 1 or state_->valid_ == true;});
        if(not state_->valid_){throw std::logic_error("future empty");}
        if(not state_->expected_.has_value()){std::rethrow_exception(state_->expected_.error());}
        return std::move(state_->expected_.value());
    }
    ~future(){
        if(state_ and 1 == state_->alive_.exchange(1,std::memory_order_relaxed)){
            delete state_;
            return;
        }
    }
};

template <FutureValue T>
class promise{
    details::shared_state<T>* state_ = new details::shared_state<T>();
    // * async read-call operation under top-level mutex
    void invoke_Callback(){
        // if not used or if used and blocked on get
        if(not state_->used_ or not state_->context_){
            return;
        }
        assert(state_->used_);
        assert(state_->valid_);
        assert(state_->context_);
        assert(state_->callback_);
        asio::post(*const_cast<asio::io_context*>(state_->context_),
          [callback = std::move(state_->callback_),
           expected = std::move(state_->expected_)]() mutable{
            callback(std::move(expected));
        });
    }
    promise(promise&) = delete; 
    void operator=(promise&other) = delete;
public:
    promise(){}
    promise(promise&&o):state_(o.state_){o.state_ = nullptr;}
    void operator=(promise&&other){
        // //assert(not state);
        this->state_ = other.state_;
        other.state_ = nullptr;
    }
    // * async read-write-call operation
    void set_value(T&& value){
        if(not state_){throw std::logic_error("use moved promise");}
        std::unique_lock lock(state_->mutex_);
        if (state_->valid_) throw std::logic_error("value or exception already set");
        state_->expected_ = std::move(value); 
        state_->valid_ = true;
        invoke_Callback();
        state_->cond_var_.notify_one();
    }
    // * async read-write-call operation
    void set_exception(std::exception_ptr&& exc){
        if(not state_){throw std::logic_error("use moved promise");}
        std::unique_lock lock(state_->mutex_);
        if (state_->valid_) throw std::logic_error("value or exception already set");
        state_->expected_ = tl::unexpected<std::exception_ptr>(std::move(exc)); 
        state_->valid_ = true;
        invoke_Callback();
        state_->cond_var_.notify_one();
    }
    // * sync with future constr and self constr
    future<T> get_future(){
        return future<T>(state_);
    }
    ~promise(){ // * async only read operation
        if(not state_){return;}
        std::unique_lock lock(state_->mutex_);
        if(not state_->valid_){
            state_->expected_ = tl::unexpected<std::exception_ptr>(std::make_exception_ptr(std::logic_error("promise value or exception not setted")));
            state_->valid_ = true;
            // ? notify only if future alive
            state_->cond_var_.notify_one();
        }
        lock.unlock();
        if(1 == state_->alive_.exchange(1,std::memory_order_relaxed)){
            delete state_;
            return;
        }
    }
};


template <cvk::FutureValue T>
struct FutureAwaiter{

    template<class io_context, class F, typename... Args>
requires std::is_invocable_r<cvk::future<T>, F, Args...>::value
    and (std::is_same_v<asio::io_context, std::remove_cv_t<std::remove_pointer_t<io_context>>>
        or std::is_same_v<asio::io_context, std::remove_cv_t<std::remove_reference_t<io_context>>>
    )
    FutureAwaiter(const io_context& context, F&& func, Args... args)
    :future(func(std::forward<Args>(args)...))
    {
        if constexpr(std::is_same_v<asio::io_context, std::remove_cv_t<std::remove_reference_t<io_context>>>){
            this->context = &context;
        }else{
            this->context = context;
        }
    }

    template<class io_context, class F, class C, typename... Args>
requires std::is_invocable_r<cvk::future<T>, F, C, Args...>::value
    and std::is_pointer<C>::value
    and requires(F method, C caller, Args... args){
        (caller->*method)(std::forward<Args>(args)...);
    }
    and (std::is_same_v<asio::io_context, std::remove_cv_t<std::remove_pointer_t<io_context>>>
        or std::is_same_v<asio::io_context, std::remove_cv_t<std::remove_reference_t<io_context>>>
    )
    FutureAwaiter(const io_context& context, F&& func, C caller, Args... args)
    :future((caller->*func)(std::forward<Args>(args)...))
    {
        if constexpr(std::is_same_v<asio::io_context, std::remove_cv_t<std::remove_reference_t<io_context>>>){
            this->context = &context;
        }else{
            this->context = context;
        }
    }


    bool await_ready()noexcept{return false;}
    void await_suspend(std::coroutine_handle<>cont){
        future.subscribe([this,cont](tl::expected<T,std::exception_ptr>&& expected){
            result = std::move(expected);
            cont();
        }, *context);
    }
    T await_resume(){
        if(result.has_value()){
            return std::move(result.value());
        }
        std::rethrow_exception(result.error());
    }

private:
    cvk::future<T> future;
    tl::expected<T,std::exception_ptr> result;
    const asio::io_context* context = nullptr;
};
}//? namespace cvk


template <cvk::FutureValue T, typename... Args>
struct std::coroutine_traits<cvk::future<T>, Args...>
{
    struct promise_type : cvk::promise<T>{
        cvk::future<T> get_return_object() noexcept
        {
            return this->get_future();
        }
 
        std::suspend_never initial_suspend() const noexcept { return {}; }
        std::suspend_never final_suspend() const noexcept { return {}; }

            // ? only move return value
        void return_value(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            // cppcheck-suppress throwInNoexceptFunction
            this->set_value(std::move(value));
        }
 
        void unhandled_exception() noexcept
        {
            // cppcheck-suppress throwInNoexceptFunction
            this->set_exception(std::current_exception());
        }
    };
};