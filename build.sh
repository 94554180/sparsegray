#!/bin/bash

# Check if 'Makefile' exists within 'build'
if [ -e Makefile ]; then
  echo "Makefile already exists, skipping CMake configuration"
else # Run 'cmake' command to configure the build
  mkdir -p "build"
  pushd "build"
  cmake ..
  popd
fi

# Build the project using 'make'
make