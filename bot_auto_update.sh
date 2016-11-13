#!/bin/bash

SRC_DIR=/home/pi/bot/src
BIN_DIR=/home/pi/bot/build
EXEC=${BIN_DIR}/pibot

while true; do
    sudo ${EXEC}
    cd ${SRC_DIR}
    git stash
    git fetch server
    git checkout production
    git reset --hard server/production
    cd ${BIN_DIR}
    make
done

