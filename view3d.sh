#!/bin/bash

# Check if the virtual environment exists
if [ ! -d ".venv" ]; then
    echo -e "\033[1;31mVirtual environment not found. Run ./setup.sh first!\033[0m"
    exit 1
fi

# Activate the virtual environment
source .venv/bin/activate

# Run 3D viewer
python3 scripts/view3d.py "$@"

# Deactivate the virtual environment
deactivate

