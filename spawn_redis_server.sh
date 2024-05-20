#!/bin/sh

set -xe
cmake --preset ReleaseCodecrafters -S . -B build/ReleaseCodecrafters
cmake --build build/ReleaseCodecrafters

exec ./build/ReleaseCodecrafters/server "$@"
