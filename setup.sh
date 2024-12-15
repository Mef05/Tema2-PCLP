#!/bin/bash

GREEN="\033[1;32m"
YELLOW="\033[1;33m"
BLUE="\033[1;34m"
RESET="\033[0m"

echo -e "${GREEN}Installing clang...${RESET}"

sudo apt-get update
sudo apt-get install -y clang clang-tidy

echo -e "${GREEN}Installing python, pip, tkinter...${RESET}"

sudo apt-get install -y python3 python3-pip python3-tk

echo -e "${GREEN}Installing virtualenv...${RESET}"

# Install virtualenv
sudo apt-get install -y python3-venv

# Create virtual environment
if [ ! -d ".venv" ]; then
    echo -e "${GREEN}Creating virtual environment in .venv ...${RESET}"
    python3 -m venv .venv
else
    echo -e "${YELLOW}Warning: directory .venv already exists!${RESET}"
fi

echo -e "${GREEN}Installing requirements for helper scripts...${RESET}"

# Activate the virtual environment
source .venv/bin/activate

# Install dependencies
python3 -m pip install -r scripts/requirements.txt

echo -e "${GREEN}Installing cpplint...${RESET}"

sudo apt-get install -y cpplint

# Deactivate the virtual environment
deactivate

echo -e "${BLUE}Setup complete. Good luck!${RESET}"

