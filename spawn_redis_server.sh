#!/bin/sh

set -xe
cmake -S . -B build/ReleaseCodecrafters -DCMAKE_BUILD_TYPE=Release "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
"
cmake --build build/ReleaseCodecrafters

exec ./build/ReleaseCodecrafters/server "$@"
