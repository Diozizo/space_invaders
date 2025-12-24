#!/bin/bash

INIT_PATH=$PWD

# ---- SDL3 ----
if [ ! -d 3rdParty/SDL3/build ]; then
    echo "--- Building SDL3 ---"
    cd 3rdParty/SDL3/
    mkdir -p build
    cd build
    cmake ..
    make -j
    cd "$INIT_PATH"
fi

# ---- SDL3_image ----
if [ ! -d "$INIT_PATH/3rdParty/SDL3_image-3.2.4/build" ]; then
    echo "--- Building SDL3_image ---"
    cd "$INIT_PATH/3rdParty/SDL3_image-3.2.4/"
    mkdir -p build
    cd build
    # Points to new SDL3 folder
    cmake .. -DSDL3_DIR=../SDL3/build
    make -j
fi

# ---- SDL3_gfx ----
if [ ! -d "$INIT_PATH/3rdParty/SDL3_gfx-1.0.1/build" ]; then
    echo "--- Building SDL3_gfx ---"
    cd "$INIT_PATH/3rdParty/SDL3_gfx-1.0.1/"
    mkdir -p build
    cd build
    # Points to new SDL3 folder
    cmake .. -DSDL3_DIR=../SDL3/build
    make -j
fi

# ---- Install deps for SDL3_ttf ----
echo "--- Installing dependencies ---"
sudo apt-get install -y libfreetype6-dev build-essential

# ---- SDL3_ttf ----
if [ ! -d "$INIT_PATH/3rdParty/SDL3_ttf/build" ]; then
    echo "--- Building SDL3_ttf ---"
    cd "$INIT_PATH/3rdParty/SDL3_ttf/"
    mkdir -p build
    cd build
    # Points to new SDL3 folder
    cmake .. -DSDL3_DIR=../SDL3/build
    make -j
fi

# ---- SDL3_mixer ----
if [ ! -d "$INIT_PATH/3rdParty/SDL3_mixer/build" ]; then
    echo "--- Building SDL3_mixer ---"
    cd "$INIT_PATH/3rdParty/SDL3_mixer/" || exit 1
    mkdir -p build
    cd build || exit 1
    # Points to new SDL3 folder
    cmake .. -DSDL3_DIR=../SDL3/build
    make -j
fi

# ---- NCURSES (New Section) ----
# Checks if the ncurses source folder exists
if [ -d "$INIT_PATH/3rdParty/ncurses" ]; then
    # Checks if we have already built it
    if [ ! -d "$INIT_PATH/3rdParty/ncurses/build" ]; then
        echo "--- Building Ncurses ---"
        cd "$INIT_PATH/3rdParty/ncurses"
        
        # Configure flags:
        # --prefix: Installs output to the local 'build' folder instead of system
        # --with-shared: Builds .so files (required for dynamic linking)
        # --without-debug: Speeds up build
        # --enable-widec: Enables wide characters (libncursesw), standard for modern terminals
        ./configure --prefix="$PWD/build" --with-shared --without-debug --without-ada --without-cxx --without-cxx-binding --enable-widec
        
        # Build and Install locally
        make -j
        make install
    fi
else
    echo "Warning: 3rdParty/ncurses directory not found. Skipping ncurses build."
fi

cd "$INIT_PATH"
echo "--- Building Main Project ---"
make -j