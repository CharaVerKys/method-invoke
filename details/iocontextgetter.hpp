#include "../hpp/iocontextgetter.h"
#include "../hpp/contextsstore.h"
#include "../tpp/contextsstore.tpp"

inline const asio::io_context* cvk::contexts::getContext(ContextTargets target){
    return cvk::ContextsStore<NUM_OF_CONTEXTS>::instance()->get_io_context(target);
}

inline const cvk::contexts::ioContextsPair cvk::contexts::getContexts(ContextTargets sender,ContextTargets receiver){
    return ioContextsPair{
        cvk::ContextsStore<NUM_OF_CONTEXTS>::instance()->get_io_context(sender),
        cvk::ContextsStore<NUM_OF_CONTEXTS>::instance()->get_io_context(receiver)
    };
}

inline const cvk::Receiver* cvk::contexts::getReceiver(ContextTargets target){
    return cvk::ContextsStore<NUM_OF_CONTEXTS>::instance()->getReceiver(target);
}