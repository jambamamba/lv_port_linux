#!/bin/bash
set -xe

#todo:
#load bgcolor of tabview from json
#show icons in tabs

source share/pins.txt
source share/scripts/helper-functions.sh

function buildpy() {
    pip3 install setuptools
    CC=clang \
    CXX=clang++ \
    LD_LIBRARY_PATH=/usr/local/lib \
    python3 setup.py build_ext --verbose\
    --build-lib='py-build' \
    --build-temp='/tmp/py-build' \
    --parallel=$(nproc)
    # python setup.py bdist_wheel
}

function buildelf() {
    local deps=(zlib debug_logger json_utils curl libssh2 utils)
    installDeps $@ deps depsdir="/usr/local"

	mkdir -p wl_protocols
	wayland-scanner client-header "/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml" "wl_protocols/wayland_xdg_shell.h"
	wayland-scanner private-code "/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml" "wl_protocols/wayland_xdg_shell.c"

    local cwd=$(pwd)
    mkdir -p x86-build
    pushd x86-build
    cmake -G Ninja -DCMAKE_PREFIX_PATH=${cwd}/cmake -DLV_USE_WAYLAND=1 -DCMAKE_BUILD_TYPE=Debug ..
    ninja install
    popd
}

function run() {
    local py_script="$(pwd)/src/examples/hello-world/hello-world.py"
    local config_json="$(pwd)/src/examples/hello-world/hello-world.json"

    parseArgs $@
    local input_file
    if [ -f ${py_script} ]; then
        input_file=${py_script}
    elif [ -f ${config_json} ]; then
        input_file=${config_json}
    else
        echo "missing input file"
        exit -1
    fi
    echo fs.inotify.max_user_watches=1048575 | sudo tee -a /etc/sysctl.conf && sudo sysctl -p
    echo "set confirm off" |sudo tee ~/.gdbinit

    # ways of running:
    pushd x86-build/bin
    gdb -ex "run" --args ./lvglsim ${input_file}
    popd

    # pushd x86-build/bin
    # ./lvglsim ../../src/configs/imageview.json
    # ./lvglsim ../../src/py/main.py #must be in x86-build/bin folder to run like this
    # popd

    # pushd src/examples/hello-world
    # LD_LIBRARY_PATH=/usr/local/lib gdb --args python hello-world.py
    # popd
}

function main() {
    buildelf
    # buildpy
    # run
}

main $@ |tee x86-build/build.log
