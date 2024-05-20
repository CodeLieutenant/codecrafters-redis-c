cmake_minimum_required(VERSION 3.14)

project(CodeCraftersRedis LANGUAGES C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
set(THREADS_PREFER_PTHREAD_FLAG ON)

include(CheckIncludeFile)
include(CheckIPOSupported)

check_include_file("stdio.h" HAVE_STDIO_H)
check_include_file("stdint.h" HAVE_STDINT_H)
check_include_file("stdlib.h" HAVE_STDLIB_H)
check_include_file("errno.h" HAVE_ERRNO_H)
check_include_file("inttypes.h" HAVE_INTTYPES_H)
check_include_file("sys/stat.h" HAVE_SYS_STAT_H)
check_include_file("sys/socket.h" HAVE_SYS_SOCKET_H)
check_include_file("sys/types.h" HAVE_SYS_TYPES_H)
check_include_file("string.h" HAVE_STRING_H)
check_include_file("unistd.h" HAVE_UNISTD_H)
check_include_file("netinet/in.h" HAVE_NETINET_IN_H)
check_include_file("netinet/ip.h" HAVE_NETINET_IP_H)

find_package(libuv CONFIG REQUIRED)
find_package(mimalloc CONFIG REQUIRED)

find_package(Threads REQUIRED)

configure_file(config.in config.h USE_SOURCE_PERMISSIONS NEWLINE_STYLE UNIX)

add_executable(server)
target_sources(server PRIVATE src/server.c config.h)
target_precompile_headers(server PRIVATE include/pch.h)
target_include_directories(server PRIVATE include "${PROJECT_BINARY_DIR}")

target_link_libraries(server PRIVATE $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,libuv::uv>)
target_link_libraries(server PRIVATE $<IF:$<TARGET_EXISTS:mimalloc-static>,mimalloc-static,mimalloc>)
target_link_libraries(server PRIVATE Threads::Threads)
target_compile_options(server PRIVATE -Werror -Wextra -Wpedantic -Wno-gnu-zero-variadic-macro-arguments)

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    target_compile_options(server PRIVATE -O0 -g -ggdb -g3 -gdwarf-4 -fcolor-diagnostics -fansi-escape-codes)
else()
    check_ipo_supported(RESULT LTO_SUPPORTED)

    if(LTO_SUPPORTED AND lto)
        message(STATUS "LTO is supported and enabled")
        set_property(TARGET ${target} PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
    endif()
endif()
