# Blip

**Blip** bridges the gap between hardware infrastructure and UI design. It is an end-to-end platform that combines a Figma-style visual drag-and-drop GUI builder with a secure fleet management stack (Secure Boot, OTA updates, and Remote Tracking).

Blip compiles your visual designs into blazing-fast, native **LVGL / C++ binaries**, and exposes them entirely via **Python bindings**. You get the 60-FPS performance of bare-metal C++ with the absolute scripting simplicity of Python.

---

## ✨ Key Features

- **Figma-for-Edge UI Builder:** Drag-and-drop buttons, sliders, video streams, and complex layouts from the Blip Web Dashboard.
- **100% Python Logic:** Control your entire UI, hardware pins, and AI models using simple Python scripts. No raw C++ memory management or pointer headache.
- **JSON Layouts:** UIs are described in plain JSON — views, labels, buttons, images, sliders, stack views, tab views, rollerviews, and message boxes — all nestable with full flexbox layout support.
- **Production-Grade Fleet Security:** Integrated Secure Boot, encrypted Over-The-Air (OTA) updates, and automated rollback protections.
- **Unified Dashboard:** Track fleet health, monitor device uptime, and view pre-instrumented crash diagnostics in real-time.
- **Zero Display Configuration:** Flash the Blip Golden Image, pair your device, and watch your UI deploy to the physical screen in under 5 minutes.

---

## 🛠 Supported Hardware

Blip runs the exact same design workflow across the entire hardware spectrum, scaling automatically based on system resources:

- **ESP32 Series:** Bare-metal, ultra-lightweight execution. Perfect for low-cost microcontrollers, smart home appliances, and wearables.
- **Raspberry Pi (Zero to 5):** Direct display server configuration (bypassing heavy X11/Wayland setups). Ideal for retail kiosks and digital signage.
- **NVIDIA Jetson (Orin Nano / Orin NX):** Designed for Edge AI. Data scientists can bind computer vision model outputs (OpenCV, PyTorch) directly to the UI using Python.

---

## 🐍 Python + JSON Apps

Blip applications are built from two files: a **JSON layout** describing the UI widget tree, and a **Python script** containing all application logic. No C++ compilation step is required for app development.

### Hello World

**`hello-world.json`** — declare a label:
```json
{
    "version":"1.0",
    "screen":{"width":"1024","height":"768"},
    "view":{
        "id":"/hello-world",
        "style":{
            "width":"100%","height":"100%","bgcolor":"#444","fgcolor":"#fff",
            "flex":{"flow":"column","justify-content":"center","align-items":"center"}
        },
        "label":{
            "id":"/hello-world/label","text":"Hello World!",
            "style":{"width":"40%","height":"10%","font-size":"40pt","text-align":"center"}
        }
    }
}
```

**`hello-world.py`** — load the layout and interact with widgets:
```python
import lele

if not lele.loadConfig("hello-world.json"):
    sys.exit(1)

label = lele.getObjectById("/hello-world/label")
print(f"label text: {label.getText()}")

while lele.handleEvents():
    pass
```

### Restaurant Ordering App

A full menu‑browsing and ordering application with 25+ items, category tabs, detail view, and bag counter — all in ~70 lines of Python and a JSON layout with style classes.

Layout features used:
- **Style classes** — reusable named style blocks (`topbar`, `card`, `detail/img`, etc.)
- **Stack views** — swipeable page stacks for master/detail navigation
- **Grid layout** — `row-wrap` flex for card grids
- **Images** — per-item food photography
- **Event binding** — `addEventHandler` connects Python callbacks to UI elements
- **Dynamic text updates** — bag count, detail panel population

### Mandelbrot Explorer

An interactive fractal zoom application driven by CUDA-accelerated rendering:

```python
import lele
import time

has_cuda = lele.hasCuda()
if has_cuda:
    lele.generateMandelbrot("res/mandelbrot.png", 800, 600)

lele.loadConfig("mandelbrot.json")
status_label = lele.getObjectById("/mandelbrot/status")
play_btn = lele.getObjectById("/mandelbrot/play")

def on_play(event):
    global playing
    playing = True
    status_label.setText("Zooming...")

play_btn.addEventHandler(on_play)

while lele.handleEvents():
    if playing:
        zoom *= 1.03
        lele.updateMandelbrotImage("/mandelbrot/image",
            1000, 4.0, real_min, real_max, imag_min, imag_max)
        time.sleep(0.033)
```

Controls: play/pause/reset zoom animation, arrow-key nudging of the viewport, and a speed slider — all bound in a few lines of Python.

---

## 🏗 Architecture

```
┌─────────────────────────────────────────────────┐
│              Blip Web Dashboard                  │
│  (Figma-style drag-and-drop UI builder +         │
│   fleet management console)                      │
└──────────────────────┬──────────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────────┐
│           Blip Cloud Compiler                    │
│  JSON Layout ──► LVGL / C++ binary               │
│  Python script bundled alongside                 │
│  Signed & encrypted for OTA                      │
└──────────────────────┬──────────────────────────┘
                       │
                       ▼
┌─────────────────────────────────────────────────┐
│           Target Device (RPi / Jetson / ESP32)   │
│                                                   │
│  ┌─────────────────────────────────────────┐      │
│  │  lvglsim (C++ Runtime Engine)           │      │
│  │  ┌──────────┐  ┌──────────────────────┐ │      │
│  │  │ LVGL GUI │  │  Python Interpreter  │ │      │
│  │  │ (native  │  │  (embedded CPython)  │ │      │
│  │  │  C++/GPU)│  │                      │ │      │
│  │  └────┬─────┘  └──────────┬───────────┘ │      │
│  │       │                   │              │      │
│  │       └───────┬───────────┘              │      │
│  │               ▼                          │      │
│  │     Python C Bindings (lele module)      │      │
│  │     • lele.loadConfig(json)              │      │
│  │     • lele.getObjectById(id)             │      │
│  │     • lele.dumpScreenshot()              │      │
│  │     • lele.handleEvents()                │      │
│  │     • lele.addEventHandler(id, cb)       │      │
│  │     • lele.generateMandelbrot(...)       │      │
│  │     • lele.hasCuda()                     │      │
│  └─────────────────────────────────────────┘      │
└───────────────────────────────────────────────────┘
```

The `lvglsim` executable embeds a CPython interpreter and pre-registers the `lele` native module via `PyImport_AppendInittab`. All UI widgets (buttons, labels, images, sliders, stack views, tab views, etc.) are exposed through the `LeleObject` C++ class hierarchy, which bridges into Python with zero-copy event forwarding.

---

## 🚀 Quick Start (Linux x86)

### Prerequisites

- CMake ≥ 3.10
- C++17 compiler (GCC ≥ 9 or Clang ≥ 10)
- Wayland development libraries
- Python 3.13 development headers (bundled)
- CUDA Toolkit ≥ 12 (optional, for Mandelbrot GPU acceleration)
- ImageMagick 7 (for screenshot PNG export)

Install dependencies:

```sh
# Ubuntu / Debian
sudo apt install cmake build-essential libwayland-dev libwayland-cursor-dev \
    libwayland-bin libxkbcommon-dev wayland-protocols libevdev-dev \
    libmagick++-dev
```

### Build

```sh
# Native x86 build
cmake -B x86-build -S .
cmake --build x86-build --target lvglsim -j$(nproc)
```

### Run an App

```sh
# JSON-only app (auto-screenshots, then enters event loop)
WAYLAND_DISPLAY=wayland-1 \
    x86-build/bin/lvglsim src/examples/gallery/gallery.json

# Python app
WAYLAND_DISPLAY=wayland-1 \
    PYTHONHOME=x86-build/src/Python-3.13.3 \
    x86-build/bin/lvglsim src/examples/hello-world/hello-world.py
```

---

## 📹 RDP Display Server

For remote development, Blip runs the UI on a Weston compositor with RDP backend:

```sh
# Start RDP Weston (port 3389)
repos/utils/share/scripts/rdp-server.sh weston

# Run an app against it
WAYLAND_DISPLAY=wayland-1 \
    x86-build/bin/lvglsim src/examples/mandelbrot/mandelbrot.py
```

Connect your RDP client to `localhost:3389` to see the display.

---

## 🔧 Cross-Compile for Raspberry Pi

Use the provided Raspberry Pi container to cross-compile:

```sh
# Build the cross-compilation container
docker build -t blip-rpi -f docker/Dockerfile.rpi .

# Run the container with the repo mounted
docker run --rm -it \
    -v $(pwd):/build \
    blip-rpi \
    bash -c "cd /build && cmake -B rpi-build -S . \
        -DCMAKE_TOOLCHAIN_FILE=./cmake/toolchain-rpi.cmake && \
        cmake --build rpi-build --target lvglsim -j\$(nproc)"
```

The resulting binary at `rpi-build/bin/lvglsim` can be deployed directly to a Raspberry Pi running Weston or a direct DRM/KMS backend.

Build dependencies required in the container:
- `gcc-arm-linux-gnueabihf` / `g++-arm-linux-gnueabihf` cross toolchain
- Wayland protocol headers and libraries for the target architecture
- CMake, pkg-config

---

## 📋 Project Status

| Issue | Status | Fix |
|---|---|---|
| Screenshots grayscale (b/w) | **Fixed** | `lv_snapshot_take` + BGRA→RGBA byte-swap (replaced partial Wayland SHM buffer read) |
| CUDA mandelbrot 57 s | **Fixed** | Bulk `thrust::host_vector`→`device_vector` copy (now ~51 ms) |
| Image widget height = 0 (flex-grow) | **Fixed** | Replaced `flex-grow:1` with explicit `height:80%` / `height:6%` |
| All 7 button labels invisible | **Fixed** | `lv_obj_set_width(label, lv_pct(100))` instead of `lv_obj_get_width(_lv_obj)` |
| `dumpScreenshot` hangs on repeated calls | **Fixed** | Removed unnecessary `lv_refr_now(NULL)` before snapshot — `loadFromData` renders independently |
| JSON layout duplicate keys | **Not a bug** | cJSON preserves all duplicate keys through round-trip; confirmed via standalone test |

### Root Cause — Button Label Bug

- JSON → cJSON → tokenize pipeline correctly preserves all 7 buttons (proven via standalone round-trip test).
- LVGL flex layout hasn't sized children 2‑7 at creation time, so `lv_obj_get_width(_lv_obj)` returns 0.
- Setting label width to 0 makes text invisible even after flex layout runs later.
- Fix: `lv_pct(100)` dynamically tracks the parent button's final width after layout.

### Snapshot Rendering

The LVGL `lv_snapshot_take` function was patched to render the full widget tree recursively instead of only the screen's background rectangle. The fix in `lvgl/src/others/snapshot/lv_snapshot.c` adds `snapshot_draw_recursive()` — a post-order traversal that calls `lv_obj_redraw` on each object, recursively renders all children (reverse order), then sends `LV_EVENT_DRAW_POST_BEGIN/MAIN/END`.

### Color Format

LVGL stores ARGB8888 as `[B, G, R, A]` on little-endian hardware. The screenshot pipeline byte-swaps bytes 0 and 2 of each pixel before passing to ImageMagick's `"RGBA"` map, producing correct full-color PNGs.

---

## 🧪 Test

```sh
# Run all unit tests
x86-build/bin/style_tests

# Integration test: auto-play mandelbrot with screenshot comparison
WAYLAND_DISPLAY=wayland-1 \
    PYTHONHOME=x86-build/src/Python-3.13.3 \
    x86-build/bin/lvglsim src/examples/mandelbrot/test_mandelbrot.py

# Manual screenshot capture
WAYLAND_DISPLAY=wayland-1 \
    PYTHONHOME=x86-build/src/Python-3.13.3 \
    x86-build/bin/lvglsim src/examples/mandelbrot/mandelbrot.py
# Press play — screenshots saved to /tmp/screenshot-*.png
```

---

## 📁 Project Layout

```
├── src/
│   ├── main.cpp                  # Entry point — dispatches JSON or Python
│   ├── graphics_backend.cpp      # Display backend, screenshot capture
│   ├── python/
│   │   ├── python_binding.cpp    # lele Python module C bindings
│   │   └── python_wrapper.cpp    # Embedded CPython lifecycle
│   ├── lelewidgets/              # Widget wrappers (button, label, image, etc.)
│   ├── mandelbrot/               # CUDA-accelerated fractal renderer
│   ├── lv_image_converter/       # ImageMagick PNG export
│   └── examples/
│       ├── hello-world/          # Minimal JSON + Python example
│       ├── restaurant-1/         # Full menu-ordering app (~70 lines Python)
│       └── mandelbrot/           # Interactive fractal explorer
├── scripts/
│   ├── run_headless.sh           # Auto-start headless Weston helper
│   ├── backend_conf.sh           # CMake helper for backend detection
│   └── gen_wl_protocols.sh       # Wayland protocol codegen
├── lvgl/                         # LVGL library (submodule)
├── lv_conf.h                     # LVGL configuration (backends, features)
└── CMakeLists.txt                # Top-level build
```
