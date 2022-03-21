# !/bin/sh

rm -r build conan

mkdir conan
cd conan
conan install .. --build missing
cd ..

mkdir build
cd build
cmake ..
cmake --build .
cd ..
