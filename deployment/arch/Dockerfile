FROM archlinux:latest AS build

WORKDIR /project

COPY . .

RUN <<EOF
pacman -Syu --noconfirm tree
tree
EOF

RUN <<EOF
pacman -Syu --noconfirm llvm clang libc++ libc++abi cmake ninja zip wget git which \
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
EOF

FROM scratch
COPY --from=build /project/bin/qlp /
ENTRYPOINT ["/qlp"]
