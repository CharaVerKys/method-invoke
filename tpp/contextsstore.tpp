#include "../hpp/contextsstore.h"

template<uint8_t numOfContexts>
cvk::ContextsStore<numOfContexts>* cvk::ContextsStore<numOfContexts>::instance(){
    static_assert(numOfContexts == NUM_OF_CONTEXTS);
    static ContextsStore<numOfContexts> instance;
    return &instance;
}

template<uint8_t numOfContexts>
template<typename... Receivers>
tl::expected<std::vector<std::function<void(std::stop_token)>>,std::exception_ptr> 
cvk::ContextsStore<numOfContexts>::registerContexts(Receivers*... receivers) noexcept{
    static_assert(sizeof...(receivers) == numOfContexts, "Invalid number of receivers");
    receivers_ = { receivers... };
    return std::vector<std::function<void(std::stop_token)>>();
}


template<uint8_t numOfContexts>
expected_ue cvk::ContextsStore<numOfContexts>::startContexts(std::vector<std::function<void(std::stop_token)>> work){
    for(uint i = 0; i<numOfContexts; ++i){
        std::function<void()> work_ = std::bind_front(work.at(i),stopSource_.get_token());
        std::function threadWork = [this, i, work_, receiver = receivers_[i]](){
            work_guards_[i].emplace(asio::make_work_guard(*receiver->private_getLoop()));
            asio::post(*receiver->private_getLoop(),work_);
            receiver->private_getLoop()->run();
        };
        std::thread thread(threadWork); // идеально! типы стёрты, токен внутри стёрт, мьютексов и т.п. нет
        threads_[i] = std::move(thread);
    }
    return {};
}

template<uint8_t numOfContexts>
void cvk::ContextsStore<numOfContexts>::requestStop(){
    stopSource_.request_stop();
    for(auto& w : work_guards_){
        w.reset();
    }
}

template<uint8_t numOfContexts>
void cvk::ContextsStore<numOfContexts>::wait(){
    for(auto& thread: threads_){
        thread.join();
    }
}

template<uint8_t numOfContexts>
cvk::ContextsStore<numOfContexts>::ContextsStore(){
    static_assert(numOfContexts not_eq 0,"");
}