#include "../hpp/iocontextgetter.h"
#include "../hpp/methodinvoker.h"
#include "../hpp/receiver.h"

inline void cvk::method::invoke(ContextTargets context, std::string_view method, std::any val) {
    const Receiver* receiver = contexts::getReceiver(context);
    assert(receiver);
    std::unique_ptr<std::any> p_val = std::make_unique<std::any>(std::move(val));
    const_cast<Receiver*>(receiver)
        ->private_invokeMethod(method, std::move(p_val));
}
