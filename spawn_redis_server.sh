#!/bin/sh

set -xe
cmake --preset ReleaseCodecrafters -S . -B build/ReleaseCodecrafters -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake
cmake --build build/ReleaseCodecrafters

exec ./build/ReleaseCodecrafters/server "$@"
