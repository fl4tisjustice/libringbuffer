cmake_minimum_required(VERSION 3.10)
project (ringqueue C CXX)
add_library(ringqueue SHARED)

set_target_properties (ringqueue PROPERTIES
    C_STANDARD 23
    CXX_STANDARD 23
    C_STANDARD_REQUIRED YES
    CXX_STANDARD_REQUIRED YES
    DEFINE_SYMBOL LIBRINGQUEUE_BUILD
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -pedantic-errors -Wpedantic -Wall -Werror -Wextra -fPIC -fvisibility=hidden")

target_sources(ringqueue PRIVATE
    ${PROJECT_SOURCE_DIR}/src/ringbuffer_interface.cpp
)

target_include_directories(ringqueue PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_options(ringqueue PRIVATE -shared -fPIC -fvisibility=hidden)

if (WIN32)
    target_link_options(ringqueue PRIVATE -static-libgcc -static-libstdc++ LINKER:-Bstatic,--whole-archive -lwinpthread LINKER:-Bdynamic,--no-whole-archive -lonecore)
endif ()

install(TARGETS ringqueue DESTINATION .)
install(FILES ${PROJECT_SOURCE_DIR}/include/ringqueue_interface.h ${PROJECT_SOURCE_DIR}/include/ringqueue.hpp DESTINATION ${PROJECT_SOURCE_DIR}/lib)