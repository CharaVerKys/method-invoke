#pragma once
#include "defines.h"
#include "receiver.h"
#include <optional>

namespace cvk{
template<uint8_t numOfContexts>
class ContextsStore{
public:
    static ContextsStore* instance();

    template<typename... Receivers>
    tl::expected<std::vector<std::function<void(std::stop_token)>>,std::exception_ptr> 
    registerContexts(Receivers*... receivers) noexcept; // ! set in order of enum contexts
    expected_ue startContexts(std::vector<std::function<void(std::stop_token)>> contexts); // ! set in order of enum contexts
    
    void requestStop();
    void wait();

    const asio::io_context* get_io_context(ContextTargets target){return receivers_[target]->private_getLoop();}
    const Receiver* getReceiver(ContextTargets target){return receivers_[target];}

private:
    ContextsStore();
    std::stop_source stopSource_; // may be stopped by sys signal ctrl+C and from workers
    std::array<Receiver*,numOfContexts> receivers_;
     std::array<std::optional<asio::executor_work_guard<asio::io_context::executor_type>>,numOfContexts> work_guards_;
    std::array<std::thread,numOfContexts> threads_;
};
}//? namespace cvk