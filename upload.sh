#! /bin/bash

ROOT_DIR=$(dirname "$0")

FIRST_BOARD_PORT=$(arduino-cli board list | tail -n -2 | awk '{print $1}')

arduino-cli upload \
    --fqbn arduino:avr:uno \
    --port ${FIRST_BOARD_PORT} \
    --input-dir "${ROOT_DIR}/target/board"
