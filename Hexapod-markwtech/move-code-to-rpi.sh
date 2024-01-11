#!/usr/bin/env bash 

SOURCE="/mnt/c/Users/Kyuubi/Repositories/Raspberry-Robotics/Hexapod-markwtech/"
DEST="hexapod@192.168.88.210:/home/hexapod/Code"

sshpass -p hexapod scp -r "$SOURCE" "$DEST"
