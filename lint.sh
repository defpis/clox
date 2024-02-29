#!/bin/bash

root=$(cd "$(dirname "$0")" && pwd)

cd "$root"

#find "src" -iname '*.h' -o -iname '*.cpp' | xargs clang-tidy -p="cmake-build-debug" --config-file=".clang-tidy"
find "src" -iname '*.h' -o -iname '*.cpp' | xargs clang-format -i
