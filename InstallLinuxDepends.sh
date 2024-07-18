#!/bin/bash

# Define Colours
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
RESET='\033[0m'

echo -e "Dependency installer for TV-Tagger"

# Function to check if a command exists
command_exists() {
    command -v "$1" &>/dev/null
}

# Initialize installation flags
install_ffmpeg=false
install_python=false

# Check if ffmpeg is installed
echo -e "${BLUE}Checking for ffmpeg${RESET}"
if command_exists ffmpeg; then
    echo -e "${GREEN}ffmpeg is already installed.${RESET}"
else
    echo -e "${RED}ffmpeg is not installed.${RESET}"
    install_ffmpeg=true
fi

# Check if python is installed
echo -e "${BLUE}Checking for python${RESET}"
if command_exists python3; then
    echo -e "${GREEN}Python is already installed.${RESET}"
else
    echo -e "${RED}Python is not installed.${RESET}"
    install_python=true
fi

# Check if any installations are needed
if $install_ffmpeg || $install_python; then
    echo -e "${YELLOW}Checking for package manager${RESET}"
    # Check if apt is available
    if command_exists apt; then
        echo -e "${YELLOW}Found apt${RESET}"
        sudo apt update
        if $install_ffmpeg; then
            sudo apt install ffmpeg -y
            echo -e "${GREEN}ffmpeg installed via apt.${RESET}"
        fi
        if $install_python; then
            sudo apt install python3 -y
            echo -e "${GREEN}Python installed via apt.${RESET}"
        fi
    # Check if pacman is available
    elif command_exists pacman; then
        echo -e "${YELLOW}Found pacman${RESET}"
        sudo pacman -Syu
        if $install_ffmpeg; then
            sudo pacman -S ffmpeg
            echo -e "${GREEN}ffmpeg installed via pacman.${RESET}"
        fi
        if $install_python; then
            sudo pacman -S python
            echo -e "${GREEN}Python installed via pacman.${RESET}"
        fi
    else
        echo -e "${RED}Error: Neither apt nor pacman found. Cannot install Python and ffmpeg.${RESET}"
        exit 1
    fi
else
    echo -e "${GREEN}Both ffmpeg and Python are installed.${RESET}"
fi
