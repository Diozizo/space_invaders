#!/bin/bash

INIT_PATH=$PWD

if [ ! -d 3rdParty/SDL3-3.2.24/build ]
then
cd 3rdParty/SDL3-3.2.24/
mkdir build
cd build
cmake ..
make -j
fi

if [ ! -d $INIT_PATH/3rdParty/SDL3_image-3.2.4/build ]
then
cd $INIT_PATH/3rdParty/SDL3_image-3.2.4/
mkdir build
cd build
cmake .. -DSDL3_DIR=../SDL3-3.2.24/build
make -j
fi

if [ ! -d $INIT_PATH/3rdParty/SDL3_gfx-1.0.1/build ]
then
cd $INIT_PATH/3rdParty/SDL3_gfx-1.0.1/
mkdir build
cd build
cmake .. -DSDL3_DIR=../SDL3-3.2.24/build
make -j
fi

cd $INIT_PATH
make -j
