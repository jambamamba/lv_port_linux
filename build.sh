#!/bin/bash
set -xe

function main() {
    sudo apt-get install -y \
        libevdev-dev \
        libwayland-dev \
        libxkbcommon-dev \
        libwayland-bin \
        wayland-protocols

	mkdir -p wl_protocols
	wayland-scanner client-header "/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml" "wl_protocols/wayland_xdg_shell.h"
	wayland-scanner private-code "/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml" "wl_protocols/wayland_xdg_shell.c"

    mkdir -p x86-build
    pushd x86-build
    cmake -DLV_USE_WAYLAND=1 ..
    make -j
    popd

    ./x86-build/bin/lvglsim
}

main $@