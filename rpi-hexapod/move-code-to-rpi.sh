#!/usr/bin/env bash 

SOURCE="/mnt/c/Users/Kyuubi/Repositories/Raspberry-Robotics/rpi-hexapod"
DEST="hexapod@192.168.88.210:/home/hexapod/Code"

scp -r "$SOURCE" "$DEST"
