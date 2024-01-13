#!/bin/bash

# Change to the desktop directory
cd ~/Desktop/cep

# Compile main1.c
gcc main1.c -o main1 -lcjson -lcurl -ljson-c

# Check if compilation is successful
if [ $? -eq 0 ]; then
    # Run the compiled program and print output to the terminal
    ./main1
else
    echo "Compilation failed."
fi

