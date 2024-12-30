set(example_sources
    src/example.cpp
    src/context1.cpp
    src/context2.cpp
    src/context3.cpp
)

add_executable(Example ${example_sources})
target_compile_options(Example PRIVATE
    -fpermissive)
target_compile_definitions(Example PRIVATE __cvk_config=<example_config.hpp>)
target_include_directories(Example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)