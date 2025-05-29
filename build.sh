#!/bin/bash
set -xe

function main() {
#    sudo apt-get install -y \
#        libevdev-dev \
#        libwayland-dev \
#        libxkbcommon-dev \
#        libwayland-bin \
#        wayland-protocols

	mkdir -p wl_protocols
	wayland-scanner client-header "/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml" "wl_protocols/wayland_xdg_shell.h"
	wayland-scanner private-code "/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml" "wl_protocols/wayland_xdg_shell.c"

    mkdir -p x86-build
    pushd x86-build
    cmake -DLV_USE_WAYLAND=1 -DCMAKE_BUILD_TYPE=Debug ..
    make -j
    popd

    ./x86-build/bin/lvglsim
}

# function buildImageConverter() {
#     pushd lv_image_converter
#     # if [ ! -d libpng ]; then
#     #     git clone https://github.com/pnggroup/libpng.git -b v1.6.48 --depth 1
#     # fi
#     mkdir -p build
#     cd build
#     cmake ..
#     make -j
#     popd

#     #./lv_image_converter/build/bin/lv_image_converter /home/oosman/Documents/NuGen/ .res
# }
# buildImageConverter

main $@