#!/bin/bash

#run frontend tests

cd build/frontend_tests/

./frontendtests

#run backend tests

cd ../backend_tests

./backendtests
