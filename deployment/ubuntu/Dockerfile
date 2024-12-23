FROM ubuntu:22.04 AS build

WORKDIR /project

COPY . .

RUN <<EOF
apt-get update
apt-get install -y lsb-release wget software-properties-common gnupg
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
./llvm.sh 18
apt-get install -y clang-18 libc++-18-dev libc++abi-18-dev cmake ninja-build git \
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

QLP_DATA_PATH=. cmake --preset=linux-pipeline
cmake --build build
EOF

FROM scratch
COPY --from=build /project/bin/qlp /
ENTRYPOINT ["/qlp"]
