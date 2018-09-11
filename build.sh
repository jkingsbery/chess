#!/bin/bash
mkdir -p build
g++ -std=c++11 emulator.cpp -framework sfml-graphics -framework sfml-window -framework sfml-system -o build/chess