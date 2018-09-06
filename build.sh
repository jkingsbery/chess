#!/bin/bash
mkdir -p build
g++ emulator.cpp -framework sfml-graphics -framework sfml-window -framework sfml-system -o build/chess