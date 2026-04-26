#!/bin/bash
set -xe

function build() {

pushd ~/repos
if [ ! -d FreeRDP ]; then
  git clone https://github.com/FreeRDP/FreeRDP.git
fi
cd FreeRDP
git checkout 23b36cd00ebf0ccd97750fcdbc9aa2f362352da7
cmake -S . -B build
sudo apt update
sudo apt install -y \
	libssl-dev \
	libavcodec-dev \
	libavformat-dev \
	libavutil-dev \
	libswscale-dev \
	libavdevice-dev \
	libkrb5-dev \
	libicu-dev \
	libx11-dev \
	xorg-dev \
	libcups2-dev \
	fuse3 \
	libfuse3-dev
cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DWITH_SDL=ON \
  -DWITH_SDL_IMAGE=ON \
  -DWITH_SDL_TTF=ON \
  -S . -B build
cmake --build build --parallel
popd
}

#build
~/repos/FreeRDP/build/client/X11/xfreerdp  /v:192.168.4.78:4000 /u:oosman /p:a

