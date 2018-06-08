#!/bin/sh

git clone https://github.com/christopherpoole/CADMesh.git ./cadmesh
cd ./cadmesh
git checkout v1.1
mkdir ./build
mkdir ./install
cd ./build
cmake ../ -DCMAKE_INSTALL_PREFIX=$PWD/../install/
make -j6
make install
cd ../
rm -r ./build