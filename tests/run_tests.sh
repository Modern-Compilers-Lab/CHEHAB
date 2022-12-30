#!/bin/bash

mkdir build

cd build

cmake -S ..

cd frontend_tests

make

mkdir out

./frontendtests

cd ../backend_tests

make

./backendtests



