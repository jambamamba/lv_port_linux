#!/bin/bash
set -xe

#todo:
#load bgcolor of tabview from json
#show icons in tabs

source share/pins.txt
source share/scripts/helper-functions.sh

# Now done in cmake:
# function buildpy() {
#     pip3 install setuptools
#     CC=clang \
#     CXX=clang++ \
#     LD_LIBRARY_PATH=/usr/local/lib \
#     python3 setup.py build_ext --verbose\
#     --build-lib='py-build' \
#     --build-temp='/tmp/py-build' \
#     -j $(nproc)
#     # python setup.py bdist_wheel
# }

function buildelf() {
    local deps=(zlib debug_logger json_utils curl libssh2 utils)
    installDeps $@ deps depsdir="/usr/local"

	mkdir -p wl_protocols
	wayland-scanner client-header "/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml" "wl_protocols/wayland_xdg_shell.h"
	wayland-scanner private-code "/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml" "wl_protocols/wayland_xdg_shell.c"

    local cwd=$(pwd)
    mkdir -p x86-build
    pushd x86-build
    cmake -G Ninja -DCMAKE_PREFIX_PATH=${cwd}/cmake -DLV_CONF_INCLUDE_SIMPLE=1 -DLV_USE_WAYLAND=1 -DCMAKE_BUILD_TYPE=Debug ..
    ninja install
    popd
}

function run() {
    local example="imageview" # hello-world | imageview | messagebox | stackview | tabview | testview
    local method="elfpy" # elfpy | elf | py

    parseArgs $@
    echo fs.inotify.max_user_watches=1048575 | sudo tee -a /etc/sysctl.conf && sudo sysctl -p
    echo "set confirm off" |sudo tee ~/.gdbinit
    local gdb=''
    if [ "${debug}" == "true" ]; then 
        gdb='gdb -ex "run" --args'
    fi

    pushd ./src/examples/${example}/
    case ${method} in
        elf)
            ${gdb} ../../../x86-build/bin/lvglsim "${example}.json"
            ;;
        elfpy)
            PYTHONHOME=../../../x86-build/bin/Python/ ${gdb} ../../../x86-build/bin/lvglsim "${example}.py"
            ;;
        py) #needs buildLeleCpython
            LD_LIBRARY_PATH=/usr/local/lib PYTHONPATH=/usr/local/lib ${gdb} python3 "${example}.py"
            ;;
        *);;
    esac
    popd
}

buildelf $@ |tee x86-build/build.log
run
