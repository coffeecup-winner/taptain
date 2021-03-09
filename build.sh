#! /bin/bash

ROOT_DIR=$(dirname "$0")

arduino-cli compile \
    --fqbn arduino:avr:uno \
    --build-property "compiler.cpp.extra_flags=-I${ROOT_DIR}/src" \
    --output-dir "${ROOT_DIR}/bin"
