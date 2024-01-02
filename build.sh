#!/bin/bash


# Retrieve libidk and libIDKengine header files
# ----------------------------------------------------------------------------------------------
mkdir ./external
cp -R ../IDKGameEngine/build/include ./external/.
cp -R ../IDKGameEngine/build/lib     ./external/.
# ----------------------------------------------------------------------------------------------


# Build IDKGameLoader
# ----------------------------------------------------------------------------------------------
mkdir -p build/CMake

cd build/CMake
cmake -G Ninja ../../
ninja -j 12
# ----------------------------------------------------------------------------------------------

cd ../../
cp ./build/IDKGameLoader ../NewIDKGame/output/IDKGameLoader
