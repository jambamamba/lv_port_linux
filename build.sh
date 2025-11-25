#!/bin/bash
set -xe

#todo:
#load bgcolor of tabview from json
#show icons in tabs

source share/pins.txt
source share/scripts/helper-functions.sh

function main() {
    local py_script="$(pwd)/src/py/main.py"
    local config_json="$(pwd)/src/configs/testview.json"
    parseArgs $@
    local input_file
    if [ -f ${py_script} ]; then
        input_file=${py_script}
    elif [ -f ${config_json} ]; then
        input_file=${config_json}
    else
        echo "syntax:"
        echo " ./build.sh py_script=/path/to/py/script"
        echo " ./build.sh config_json=/path/to/config/json"
        exit -1
    fi

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

    echo fs.inotify.max_user_watches=1048575 | sudo tee -a /etc/sysctl.conf && sudo sysctl -p

    # pushd x86-build/bin
    # echo "set confirm off" |sudo tee ~/.gdbinit
    # gdb -ex "run" --args ./lvglsim ${input_file}
    # popd

    #ways of running:
    # cd x86-build/bin
    # ./lvglsim ../../src/configs/imageview.json
    # ./lvglsim ../../src/py/main.py #must be in x86-build/bin folder to run like this
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

main $@ |tee x86-build/build.log