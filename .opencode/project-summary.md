# LVGL Port Linux — Project Summary

## Overview

A heavily extended fork of LVGL's `lv_port_linux` reference. It builds a GUI simulator for embedded Linux devices using LVGL, with a custom widget framework (`lelewidgets`) and a Python binding (`lele` module). The end-user programming model is declarative JSON scene graphs driven by Python scripts.

## Directory Structure

```
/
├── CMakeLists.txt              # Top-level — configures LVGL + backends, subdirs into src/
├── lv_conf.h / lv_conf.defaults # LVGL compile-time configuration
├── build.sh                    # Build + run script: cmake → ninja → install
├── setup.py                    # CPython extension builder for lele.so
├── src/
│   ├── main.cpp                # Entry point: loads .json directly or .py via PythonWrapper
│   ├── CMakeLists.txt          # Builds lvglsim, lele_ui.so, Python bindings, tests, fonts, images
│   ├── lelewidgets/            # Custom widget framework (LeleLabel, LeleButton, etc.)
│   │   ├── lele<widget>.h      # C++ class declaration
│   │   ├── lele<widget>.cpp    # C++ impl wrapping lv_obj_t
│   │   ├── lele<widget>.py.cpp # CPython binding (PyType, init, methods)
│   │   ├── lelewidgetfactory   # JSON → widget tree parser
│   │   └── tests/              # GTest test suite
│   ├── python/                 # Python integration
│   │   ├── python_wrapper.cpp  # Embeds CPython, loads .py scripts
│   │   ├── python_binding.cpp  # PyInit_lele, module-level functions
│   │   └── python_helper.cpp   # Utility helpers
│   ├── json/json_helper.cpp    # JSON parsing utilities
│   ├── font/                   # Font loading, ttf→lvf conversion
│   ├── tr/                     # Translation/i18n support
│   ├── image_builder/          # Image processing toolchain
│   ├── lv_image_converter/     # LVGL image format converter
│   ├── graphics_backend.cpp/h  # Display/input event loop abstraction
│   ├── lib/                    # LVGL Linux display/input backends
│   │   ├── display_backends/   # fbdev, DRM, SDL2, Wayland, X11, OpenGLES
│   │   └── indev_backends/     # EVDEV input
│   ├── res/                    # Resources (images, fonts)
│   └── examples/               # Example applications
│       ├── hello-world/        # Minimal label, i18n text
│       ├── hello-world-international/  # Same with language switching
│       ├── hello-world-flags/  # Language flags, font switching
│       ├── stackview/          # Navigation stack (push/pop views)
│       ├── tabview/            # Tab-based navigation
│       ├── imageview/          # Image gallery with color wheel
│       ├── restaurant-1/       # Full restaurant menu app (complex)
│       ├── apple-info/         # Product info cards
│       ├── messagebox/         # Modal dialog example
│       └── testview/           # Test view (json only, no .py)
├── lvgl/                       # LVGL library submodule
├── x86-build/                  # Build output directory (ninja-based)
├── py-build/                   # Python extension build temp
└── .vscode/launch.json         # VSCode launch config for debugging
```

## How to Build

Run `./build.sh` which:

1. Installs dependencies via `helper-functions.sh` (zlib, debug_logger, json_utils, curl, libssh2, utils)
2. Generates Wayland protocol headers via `wayland-scanner`
3. Runs CMake with Ninja (`cmake -G Ninja ...`) in `x86-build/`
4. Builds with `ninja install`

The build pipeline produces:
- `x86-build/bin/lvglsim` — the main executable (~200KB)
- `x86-build/src/liblele_ui.so` — shared library with all widgets
- `lele.cpython-314-x86_64-linux-gnu.so` — CPython extension (via setup.py)
- `x86-build/bin/Python/` — bundled CPython 3.13.3 (downloaded & compiled during build)
- Converted fonts and images

## How to Run

### Via VSCode (launch.json)

Select an example from the pick list (`hello-world`, `restaurant-1`, `stackview`, etc.) and launch. The debugger runs:

```
lvglsim src/examples/<example>/<example>.py
```

with `PYTHONHOME` pointing to the bundled Python and `cwd` set to the example's directory.

### Via build.sh

Edit `build.sh` to set `example="restaurant-1"` and `method="elfpy"` (default), then run:

```bash
./build.sh run
```

Three execution modes:
| Mode | Command | Description |
|------|---------|-------------|
| `elf` | `lvglsim <example>.json` | JSON-only, no Python |
| `elfpy` | `lvglsim <example>.py` | Python script via embedded CPython |
| `py` | `python3 <example>.py` | Pure Python using lele.so |

## Main Entry Point (`src/main.cpp`)

- Extension `.json` — calls `LeleWidgetFactory::fromConfig()` directly, enters the LVGL event loop.
- Extension `.py` — calls `PythonWrapper::load()`, which embeds CPython 3.13 and runs the script.

## Example Pattern

Every example follows the same layout:

```
<example>/
  <example>.json       # Scene graph (declarative UI)
  <example>.py         # Runtime entry (imperative glue)
  <example>.png        # Screenshot
  tr/<lang>/...        # Translation cache (optional)
  res/, flags/         # Local assets (optional)
```

### Python skeleton

```python
import lele                         # C extension
res = lele.loadConfig("example.json")  # Parse JSON → build widget tree
if not res:
    sys.exit(1)
# Optional: lele.getObjectById("...").addEventHandler(lambda e: ...)
while lele.handleEvents():
    pass
```

### JSON schema

| Key | Purpose |
|-----|---------|
| `version`, `screen{width,height}` | Top-level metadata |
| `language{current,default}` | Internationalization |
| `style{class:..., ...}` | Reusable style classes (CSS-like) |
| `view`, `label`, `button`, `img` | Widget types (map 1:1 to `Lele*` C++ classes) |
| `messagebox`, `stackview`, `tabview` | Complex container widgets |
| `id` | Hierarchical path-style ID (e.g., `/hello-world/label`) |
| `event{type,action,args}` | Declarative inline event (no Python needed) |
| `text_arabic`, `text_chinese_simplified`, ... | Per-language label overrides |
| `button type` | `"push"` (default), `"checkbox"`, `"radio"`, `"switch"`, `"slider"`, `"slider-range"`, `"color-picker"`, `"arc"`, `"close"` |
| `"#key": "..."` | Comment key (survives parsing, ignored) |

### Declarative events

Simple interactions can live entirely in JSON:

```json
"event":{"type":"clicked","action":"stackview.push","args":{"id":"views:1/view"}}
```

Actions include: `stackview.push`, `stackview.pop`, and Python callbacks registered via `lele.addEventHandler()`.

## Widget Framework (`src/lelewidgets/`)

Strict 1:1:1 file pattern per widget:

| File | Content |
|------|---------|
| `lele<widget>.h` | C++ class (e.g., `LeleButton`) |
| `lele<widget>.cpp` | C++ implementation wrapping `lv_obj_t` |
| `lele<widget>.py.cpp` | CPython type binding |

Full widget list: `button`, `label`, `view`, `image`, `colorwheel`, `messagebox`, `stackview`, `tabview`, `textbox`, `rollerview`, `nullwidget`, plus `style` and `event` as first-class objects. Every widget follows the 1:1:1 pattern; `rollerview` has all three files present.

`LeleObject` is the base class. `LeleWidgetFactory::fromConfig()` parses JSON and builds the widget tree.

## Screenshot Auto-Capture

Both execution modes automatically capture a screenshot on startup:

- **`.json` mode** (`main.cpp:63-67`): runs 20 `handleEvents()` ticks to let LVGL render, then calls `dumpScreenshot()` → `/tmp/screenshot-0.png`.
- **`.py` mode** (`python_binding.cpp:213-218`): creates a one-shot `lv_timer` that fires after 3 seconds to call `dumpScreenshot()` → `/tmp/screenshot-0.png`.

The `dumpScreenshot()` function (`graphics_backend.cpp:89-112`) uses `lv_snapshot_take()` with `LV_COLOR_FORMAT_RGB888` and saves via `ImgHelper::saveToFile()`. Output files rotate up to `/tmp/screenshot-99.png`.

## Image Builder

The `src/image_builder/` module processes images at runtime when loaded from JSON. It handles:
- Loading PNG files via ImageMagick (`img_helper`)
- Cropping to fit widget dimensions (e.g., food cards cropped to 317×176)
- Color format conversion (RGB888, ARGB8888)
- Background color fills and rotation
- Style key filtering (`img/class`, `img/id`, etc.)

## Supported Display Backends

Configured via `lv_conf.h` flags (`LV_USE_*`):

- **SDL2** — software renderer, no GPU needed
- **DRM/KMS** — direct Linux graphics
- **FBDEV** — Linux framebuffer
- **Wayland** — compositor-based (primary target, currently the only enabled backend)
- **X11** — X Window System
- **OpenGLES** — GPU-accelerated via GLFW3/GLEW
- **EVDEV** — input via libevdev

Wayland requires a running compositor. For headless testing, start Weston:
```bash
weston --backend=headless-backend.so --socket=wayland-0 &
WAYLAND_DISPLAY=wayland-0 lvglsim example.py
```

## Tests

Located in `src/lelewidgets/tests/`, built with Google Test, run via:

```bash
ctest --output-on-failure
# or
./x86-build/src/lelewidgets/tests/style_tests
```
