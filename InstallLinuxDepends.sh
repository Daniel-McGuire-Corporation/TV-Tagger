#!/bin/bash

# Check if apt is available
if command -v apt &>/dev/null; then
    sudo apt update
    sudo apt install python3 ffmpeg
    echo "Python installed via apt."
# Check if pacman is available
elif command -v pacman &>/dev/null; then
    sudo pacman -Syu
    sudo pacman -S python ffmpeg
    echo "Python installed via pacman."
else
    echo "Error: Neither apt nor pacman found. Cannot install Python."
    exit 1
fi
