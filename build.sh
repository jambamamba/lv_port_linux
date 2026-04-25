#!/bin/bash
set -xeuo pipefail #better for bash debugging
script_dir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

#todo:
#load bgcolor of tabview from json
#show icons in tabs

source ${script_dir}/share/pins.txt
source ${script_dir}/share/scripts/helper-functions.sh

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
    echo "#!/bin/bash
set -xe

cmake -G Ninja\
 -DCMAKE_PREFIX_PATH=${cwd}/cmake \
 -DLV_CONF_INCLUDE_SIMPLE=1 \
 -DLV_USE_WAYLAND=1 \
 -DCMAKE_BUILD_TYPE=Debug ..
" > cmake.sh
    chmod +x cmake.sh
    export CC=$(which clang-20)
    export CXX=$(which clang++-20)
    ./cmake.sh
    ninja install
    popd
}

function startRdpServer() {

    local pid=$(ps -xa|grep westo[n] || true);
    if [ -z "$pid" ]; then
        export WAYLAND_DISPLAY=$(ls $XDG_RUNTIME_DIR/wayland-?)
        if [ ! -z $WAYLAND_DISPLAY ]; then
            sudo rm -f $WAYLAND_DISPLAY
        fi
        # sudo killall weston || true
        sudo -E weston --backend=rdp-backend.so --rdp-tls-cert=/usr/local/weston.keys/server.crt --rdp-tls-key=/usr/local/weston.keys/server.key --address=0.0.0.0 --port=3389 &
        echo "Starting RDP server..."
        sleep 5	
    fi
	#configure to allow remote viewing:
	export WAYLAND_DISPLAY=$(ls $XDG_RUNTIME_DIR/wayland-?)
	sudo chown $(id -u):$(id -g) $WAYLAND_DISPLAY
}

function run() {
    pushd ${script_dir}
    local example="restaurant-1" # hello-world | hello-world-international | imageview | messagebox | stackview | tabview | testview | restaurant-1
    local method="elfpy" # elfpy | elf | py
    local debug="true"
    export LD_LIBRARY_PATH="/usr/local/lib"

    startRdpServer
    parseArgs $@
    echo fs.inotify.max_user_watches=1048575 | sudo tee -a /etc/sysctl.conf && sudo sysctl -p
    echo "set confirm off" |sudo tee ~/.gdbinit
    local gdb=''
    if [ "${debug}" == "true" ]; then 
        gdb='gdb -ex run --args'
        # gdb='gdb --args'
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
    popd #./src/examples/${example}/
    popd #script dir
}

#buildelf $@ |tee x86-build/build.log
run
