set(example_sources
    src/*.cpp
)

add_executable(Example example_sources)
target_compile_definitions(Example PRIVATE __cvk_config=<example_config.hpp>)
target_include_directories(Example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)