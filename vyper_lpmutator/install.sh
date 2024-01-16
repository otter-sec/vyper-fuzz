CWD=$PWD

sudo apt install -y ninja-build liblzma-dev libz-dev pkg-config autoconf libtool wget curl unzip clang

wget https://github.com/fmtlib/fmt/releases/download/10.1.0/fmt-10.1.0.zip
unzip fmt-10.1.0.zip
rm fmt-10.1.0.zip
mv fmt-10.1.0 fmt

wget https://github.com/Kitware/CMake/releases/download/v3.28.0-rc3/cmake-3.28.0-rc3-linux-aarch64.tar.gz
tar -xzf cmake-3.28.0-rc3-linux-aarch64.tar.gz
rm -rf cmake-3.28.0-rc3-linux-aarch64.tar.gz

#git clone https://github.com/google/libprotobuf-mutator.git
wget https://github.com/google/libprotobuf-mutator/archive/refs/tags/v1.1.tar.gz -O libprotobuf-mutator.tar.gz
tar -xvzf libprotobuf-mutator.tar.gz
mv libprotobuf-mutator-1.1 libprotobuf-mutator
cd libprotobuf-mutator
mkdir build
cd build
../../cmake-3.28.0-rc3-linux-aarch64/bin/cmake .. -GNinja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DLIB_PROTO_MUTATOR_DOWNLOAD_PROTOBUF=ON
ninja
cd ../..

cd src/
mkdir proto
../libprotobuf-mutator/build/external.protobuf/bin/protoc vyper.proto --cpp_out=proto
cd ..


git clone https://gitlab.com/libeigen/eigen.git
git clone https://github.com/pybind/pybind11.git

mkdir build
cd build

../cmake-3.28.0-rc3-linux-aarch64/bin/cmake .. -GNinja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DLIB_PROTO_MUTATOR_DOWNLOAD_PROTOBUF=ON -DProtobuf_INCLUDE_DIR=$CWD/libprotobuf-mutator/build/external.protobuf/include -DProtobuf_LIBPATH=$CWD/libprotobuf-mutator/build/external.protobuf/lib
ninja
mv vyper_lpmutator_wrapper.cpython-310-aarch64-linux-gnu.so VyperFuzzLib.so

cd ..
