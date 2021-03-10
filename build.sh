#!/usr/bin/env bash

set -xeuo pipefail

cd "$(dirname "$0")"

git submodule update --init --recursive

npm i
npm run asbuild

mkdir -p build && cd build

: -------------------------------------------------------------
: NOTE: The CMake config step might appear hung when run via
:       this script - just give it a second.
: -------------------------------------------------------------

cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .

printf '\n\n\n'

./repro
