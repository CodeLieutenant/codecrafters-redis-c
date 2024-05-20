#!/bin/sh

set -xe
cmake --preset ReleaseCodecrafters -S .
cmake --build build/ReleaseCodecrafters

exec ./build/ReleaseCodecrafters/server "$@"
