#pragma once
namespace test {
    inline static bool success = false;
    inline static const char* currentTest = nullptr;
}

#define START_TEST(name) do{test::success = true; test::currentTest = #name;}while(0);
#define EXPECT(result, description) \
    if(not result) \
    {std::cerr << description << '\n'; test::success = false;}
#define COMPLETE_TEST if(test::success) \
    {std::cout << "test pass: " << test::currentTest << std::endl;} \
    else \
    {std::cerr << "test failed: "<<test::currentTest<<std::endl; \
     std::abort();}