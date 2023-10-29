sudo apt-get install protobuf-compiler libprotobuf-dev binutils cmake \
  ninja-build liblzma-dev libz-dev pkg-config autoconf libtool

wget https://github.com/fmtlib/fmt/releases/download/10.1.0/fmt-10.1.0.zip
unzip fmt-10.1.0.zip
rm fmt-10.1.0.zip
cd fmt-10.1.0
mkdir build
cd build
cmake ..
cd ../..

git clone https://github.com/google/libprotobuf-mutator.git
cd libprotobuf-mutator
curl https://github.com/Kitware/CMake/releases/download/v3.28.0-rc3/cmake-3.28.0-rc3-linux-aarch64.sh|sh
mkdir build
cd build
../cmake-3.28/cmake-3.28.0-rc3-linux-aarch64/bin/cmake .. -GNinja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug -DLIB_PROTO_MUTATOR_DOWNLOAD_PROTOBUF=ON
ninja check
ninja
cd ../..
