#!/bin/bash
set -xe

function rdpServer() {

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

rdpServer
