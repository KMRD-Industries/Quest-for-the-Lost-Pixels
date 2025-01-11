FROM ubuntu:24.04

ARG APPIMAGE_FILES=./deployment/appimage
ARG BINARY_PATH=/project/bin/qlp

ENV APP_DIR=AppDir
ENV QLP_DATA_PATH=./assets

WORKDIR /project

COPY . .

RUN <<EOF
apt-get update
apt-get install -y lsb-release wget software-properties-common gnupg
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
./llvm.sh 18
apt-get install -y clang-18 lld-18 libc++-18-dev libc++abi-18-dev cmake ninja-build git gawk \
         libxrandr-dev \
         libx11-dev \
         libxi-dev \
         libxext-dev \
         libxcb1-dev \
         libx11-xcb-dev \
         libxinerama-dev \
         libxcursor-dev \
         libfreetype-dev \
         libgl1-mesa-dev \
         libflac-dev \
         libvorbis-dev \
         libopenal-dev \
         libudev-dev

cmake --preset=linux-pipeline
cmake --build build

mkdir -p $APP_DIR/usr/bin $APP_DIR/usr/lib
cp $BINARY_PATH $APP_DIR/usr/bin/

cp $APPIMAGE_FILES/* $APP_DIR

for lib in $(ldd $BINARY_PATH | awk '{ if ($3 ~ /^\//) print $3 }'); do
    cp $lib $APP_DIR/usr/lib/
done
EOF

ENTRYPOINT cp -r $APP_DIR /output
