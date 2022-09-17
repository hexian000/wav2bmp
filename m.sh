#!/bin/sh

set -e

rm -rf build
mkdir -p build && cd build
cmake -GNinja \
    -DCMAKE_BUILD_TYPE="RelWithDebInfo" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
    ..

TARGET="$1"
if [ -n "${TARGET}" ]; then
    cmake --build . --target "${TARGET}"
else
    cmake --build .
fi
