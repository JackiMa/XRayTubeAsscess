#! /bin/bash 
# Purpose: To make the project again

# check whether this script is in the build directory
if [ "$(basename "$PWD")" != "build" ]; then
    echo "Please run this script in the build directory"
    exit 1
fi

rm -rf * && cmake .. && make -j8