#!/bin/bash

SRC_DIR=/home/pi/bot/src
BIN_DIR=/home/pi/bot/build
EXEC=${BIN_DIR}/bot

while true; do
    sudo ${EXEC}
    cd ${SRC_DIR}
    git checkout production
    git pull
    cd ${BIN_DIR}
    make
done

