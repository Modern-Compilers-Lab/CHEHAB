#!/bin/bash

mkdir build

cd build

cmake -S ..

cd frontend_tests

make

mkdir out

cd ../backend_tests

make
