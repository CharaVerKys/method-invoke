#pragma once
#include <asio/io_context.hpp>
#include "defines.h"
#include "receiver.h"

namespace cvk{
namespace contexts{
    struct [[nodiscard]] ioContextsPair{
        const asio::io_context* s; // ? aka sender
        const asio::io_context* r; // ? aka receiver
    };
    const asio::io_context* getContext(ContextTargets);
    const ioContextsPair getContexts(ContextTargets,ContextTargets);
    const Receiver* getReceiver(ContextTargets);
}
}//? namespace cvk