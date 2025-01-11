FROM archlinux:base-devel

ARG APPIMAGE_FILES=./deployment/appimage
ARG BINARY_PATH=/project/bin/qlp

ENV APP_DIR=AppDir
ENV QLP_DATA_PATH=./assets

WORKDIR /project

COPY . .

RUN <<EOF
pacman -Syu --noconfirm llvm clang lld libc++ libc++abi cmake ninja wget git which gawk \
           libxrandr \
           libx11 \
           libxi \
           libxext \
           libxcb \
           libxinerama \
           libxcursor \
           libvorbis \
           freetype2 \
           mesa \
           flac \
           openal

ln -s $(which clang) /usr/bin/clang-18
ln -s $(which clang++) /usr/bin/clang++-18

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
