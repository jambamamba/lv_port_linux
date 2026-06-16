#!/bin/bash
# Start headless Weston (if not running) and run the LVGL app
set -e

SOCKET="headless-0"
XDG_RUNTIME_DIR="${XDG_RUNTIME_DIR:-/run/user/$(id -u)}"
SOCKET_PATH="${XDG_RUNTIME_DIR}/${SOCKET}"

cleanup() {
    if [ -n "$WESTON_PID" ] && kill -0 "$WESTON_PID" 2>/dev/null; then
        echo "Stopping headless Weston (PID $WESTON_PID)..."
        kill "$WESTON_PID" 2>/dev/null
        wait "$WESTON_PID" 2>/dev/null
    fi
}
trap cleanup EXIT

# Start headless Weston if not already running
if ! pgrep -f "weston.*headless.*${SOCKET}" > /dev/null; then
    echo "Starting headless Weston on socket '${SOCKET}'..."
    weston --backend=headless-backend.so --socket="${SOCKET}" --width=1024 --height=768 &
    WESTON_PID=$!
    for i in {1..10}; do
        if [ -S "$SOCKET_PATH" ]; then
            break
        fi
        sleep 0.5
    done
    if [ ! -S "$SOCKET_PATH" ]; then
        echo "ERROR: headless Weston socket '${SOCKET}' not found"
        exit 1
    fi
    echo "Headless Weston ready (PID $WESTON_PID)"
else
    echo "Headless Weston already running on socket '${SOCKET}'"
fi

export WAYLAND_DISPLAY="${SOCKET}"
exec "$@"
