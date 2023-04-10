#!/bin/bash

# Change directory to 'build'
cd build

# Check if 'Makefile' exists within 'build'
if [ -e Makefile ]; then
  echo "Makefile already exists, skipping CMake configuration"
else
  # Run 'cmake' command to configure the build
  cmake ..
fi

# Build the project using 'make'
make
