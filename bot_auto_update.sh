#!/bin/bash

SRC_DIR=/home/pi/bot/src
BIN_DIR=/home/pi/bot/build
EXEC=${BIN_DIR}/bot/pibot

while true; do
    sudo ${EXEC}
    cd ${SRC_DIR}
    git stash
    git fetch origin
    git checkout production
    git reset --hard origin/production
    cd ${BIN_DIR}
    make pibot
done

