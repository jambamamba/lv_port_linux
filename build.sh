#!/bin/bash
set -xe

#todo:
#load bgcolor of tabview from json
#show icons in tabs

source share/pins.txt
source share/scripts/helper-functions.sh

function main() {
#    sudo apt-get install -y \
#        libevdev-dev \
#        libwayland-dev \
#        libxkbcommon-dev \
#        libwayland-bin \
#        wayland-protocols

    local deps=(zlib debug_logger json_utils curl libssh2 utils)
    installDeps $@ deps depsdir="/usr/local"

	mkdir -p wl_protocols
	wayland-scanner client-header "/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml" "wl_protocols/wayland_xdg_shell.h"
	wayland-scanner private-code "/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml" "wl_protocols/wayland_xdg_shell.c"

    local cwd=$(pwd)
    mkdir -p x86-build
    pushd x86-build
    cmake -G Ninja -DCMAKE_PREFIX_PATH=${cwd}/cmake -DLV_USE_WAYLAND=1 -DCMAKE_BUILD_TYPE=Debug ..
    ninja
    popd

    echo fs.inotify.max_user_watches=1048575 | sudo tee -a /etc/sysctl.conf && sudo sysctl -p

    pushd x86-build/bin
    echo "set confirm off" |sudo tee ~/.gdbinit
    gdb ./lvglsim
    popd
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