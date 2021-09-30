#!/bin/bash

mkdir cmakeBuild
cd cmakeBuild
cmake ..
make
./runTests
