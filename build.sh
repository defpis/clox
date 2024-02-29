#!/bin/bash

root=$(cd "$(dirname "$0")" && pwd)
build="${root}/cmake-build-debug"

do_make() {
  if [ -d "${build}" ]; then
    rm -rf "${build}"
  fi

  mkdir "${build}" && cd "${build}"
  cmake .. && make
}

do_run() {
  cd "${build}"
  cmake .. && make lox_run
  echo "~/Desktop/clox/cmake-build-debug/lox_run ~/Desktop/clox/test/test.lox"
  ~/Desktop/clox/cmake-build-debug/lox_run ~/Desktop/clox/test/test.lox
}

if [ "$1" = "make" ]; then
  do_make
fi

if [ "$1" = "run" ]; then
  do_run
fi
