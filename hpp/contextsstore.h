#pragma once
#include "defines.h"
#include "receiver.h"

namespace cvk{
template<uint8_t numOfContexts>
class ContextsStore{
public:
    static ContextsStore* instance();

    template<typename... Receivers>
    tl::expected<std::vector<std::function<void(std::stop_token)>>,std::exception_ptr> 
    registerContexts(Receivers*... receivers); // ! set in order of enum contexts
    tl::expected<Unit, std::exception_ptr> startContexts(std::vector<std::function<void(std::stop_token)>> contexts); // ! set in order of enum contexts
    
    void requestStop(){stopSource_.request_stop();}
    void wait();

    const asio::io_context* get_io_context(ContextTargets target){return receivers_[target]->private_getLoop();}
    const Receiver* getReceiver(ContextTargets target){return receivers_[target];}

private:
    ContextsStore();
    std::stop_source stopSource_; // may be stopped by sys signal ctrl+C and from workers
    std::array<Receiver*,numOfContexts> receivers_;
    std::array<std::thread,numOfContexts> threads_;
};
}//? namespace cvk