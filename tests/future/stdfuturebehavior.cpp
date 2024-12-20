#include <thread>
#include <cvk.hpp>
#include <cvktest.hpp>
// #include "../../cvk.hpp"
// #include "../testing/cvktest.hpp"

namespace StdFutureBehavior{
    void blockingOnGet_v1(){
        using namespace std::chrono_literals;
        cvk::promise<int*> promise;
        cvk::future<int*> future = promise.get_future();
        std::thread thread([promise = std::move(promise)]() mutable{
            std::this_thread::sleep_for(1s);
            try{
                promise.set_value(new int(42));
            }catch(...){
                EXPECT(false, "first set cause exception");
            }
            try{
                promise.set_value(new int(42));
            }catch(...){
                return;
            }
            EXPECT(false, "second set not cause exception");
        });
        thread.detach();
        try{
            const int* res = future.get();
            EXPECT((*res == 42), "value not 42")
        }catch(...){
            EXPECT(false,"get cause exception")
        }
        try{
            int* res = future.get();
            (void)res;
        }catch(...){
            return;
        }
        EXPECT(false,"get second time not cause exception")
    }
};

int main(){
    START_TEST("block on get with sleep in thread")
    StdFutureBehavior::blockingOnGet_v1();
    COMPLETE_TEST
}