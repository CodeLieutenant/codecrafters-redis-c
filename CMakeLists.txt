cmake_minimum_required(VERSION 3.14)

project(redis-starter-c LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

include(CheckIncludeFile)
include(CheckIPOSupported)

if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

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

configure_file(config.in config.h USE_SOURCE_PERMISSIONS NEWLINE_STYLE UNIX)

add_executable(server)
target_sources(server PRIVATE src/server.c config.h)
target_include_directories(server PRIVATE include "${PROJECT_BINARY_DIR}")

target_compile_options(server PRIVATE -Werror -Wextra -Wpedantic)

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    target_compile_options(server PRIVATE -O0 -g -ggdb -g3 -gdwarf-4 -fcolor-diagnostics -fansi-escape-codes)
else()
    check_ipo_supported(RESULT LTO_SUPPORTED)

    if(LTO_SUPPORTED AND lto)
        message(STATUS "LTO is supported and enabled")
        set_property(TARGET ${target} PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
    endif()
endif()
