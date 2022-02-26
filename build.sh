# !/bin/sh

rm -r build

mkdir build
cd build
cmake -D CMAKE_CXX_COMPILER=g++ ..
cmake --build .
cd ..

g++ -v
