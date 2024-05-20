#!/bin/sh

set -xe
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
cmake --build build

exec ./build/server "$@"
