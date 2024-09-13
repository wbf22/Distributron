# used to clean the build if something is messed up
rm -r build/*
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..

sh ../debug.sh