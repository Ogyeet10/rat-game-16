#!/bin/sh
set -eu

if ! command -v emcc >/dev/null 2>&1; then
  EMSDK_DIR="${EMSDK_DIR:-/tmp/emsdk}"
  if [ ! -d "$EMSDK_DIR" ]; then
    git clone --depth 1 https://github.com/emscripten-core/emsdk.git "$EMSDK_DIR"
  fi
  "$EMSDK_DIR/emsdk" install latest
  "$EMSDK_DIR/emsdk" activate latest
  . "$EMSDK_DIR/emsdk_env.sh"
fi

./compile-wasm.sh
