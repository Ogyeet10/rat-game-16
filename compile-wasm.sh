#!/bin/sh
set -eu

mkdir -p ./dist/wasm

emcc -std=c++20 -Wall -I./src ./src/main.cc \
  --preload-file assets \
  --shell-file ./wasm/shell.html \
  -sALLOW_MEMORY_GROWTH=1 \
  -sNO_EXIT_RUNTIME=1 \
  -sEXPORTED_FUNCTIONS='["_main","_rat_key"]' \
  -sEXPORTED_RUNTIME_METHODS='["ccall"]' \
  -o ./dist/wasm/index.html
