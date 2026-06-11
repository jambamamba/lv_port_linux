# CLAUDE.md

This file gives Claude Code orientation on the non-obvious parts of this project.

## What this project is

A heavily extended fork of LVGL's `lv_port_linux` reference. On top of the stock
LVGL/Linux backends, it adds a custom widget framework (`lelewidgets`) and a
Python binding (the `lele` module, built into `lele.cpython-3xx-*.so`).

The end-user-facing programming model is:

- Describe the UI as a **JSON scene graph**.
- Drive it from a **Python script** that does only what JSON cannot
  (event callbacks, dynamic state, language/font swaps).
- The C++ side (`src/lelewidgets/`) is the runtime that parses JSON, builds
  LVGL widgets, and exposes them to Python.

## The example pattern (`src/examples/`)

Every example folder follows the same layout:

```
<example>/
  <example>.json   ← scene graph (declarative)
  <example>.py     ← runtime entry (imperative glue)
  <example>.png    ← screenshot
  tr/<lang>/...    ← optional translation cache
  flags/, *.png    ← optional local assets
```

Every `.py` is built on the same skeleton:

```python
import lele
res = lele.loadConfig("<example>.json")
if not res:
    sys.exit(1)
# optional: lele.getObjectById("...").addEventHandler(lambda e: ...)
while lele.handleEvents():
    pass
```

The simplest examples (`apple-info`, `restaurant-1`, `stackview`, `tabview`)
keep the `.py` at exactly that skeleton — all UI and even button actions live
in the JSON via `"event":{"type":"clicked","action":"stackview.push",...}`.

The richer examples (`imageview`, `hello-world-flags`, `messagebox`) add
Python callbacks for things JSON can't express: dynamic font selection per
language, animated test loops, button-click branching, color-wheel updates.

## JSON schema conventions

- **Top-level metadata:** `version`, `screen{width,height}`,
  `language{current,default}`.
- **Reusable style classes:** repeated top-level `"style":{"class":"pane",...}`
  entries. Widgets reference them with `"class":"pane"` — CSS-like.
- **Widget keys map 1:1 to lele C++ types:** `view`, `label`, `button`, `img`,
  `messagebox`, `stackview`, `tabview`, `tabs[]`, `views[]`, `textbox`,
  `colorwheel`, `tab_header`, `view_header`.
- **Hierarchical string IDs:** path-style like `/hello-world/label`,
  `/pane3/img`, `views:1/view`. These are what `lele.getObjectById(...)`
  resolves at runtime — JSON and Python share one addressing scheme.
- **Inline declarative events:**
  `"event":{"type":"clicked","action":"stackview.push","args":{...}}`
  so simple interactions need no Python at all.
- **Per-language label overrides:** `text_arabic`, `text_chinese_simplified`,
  etc., paired with `language{current}` for i18n.
- **Disabled keys via `#` prefix:** `"#font-family":"..."`, `"#border":"none"`
  — survives parsing but is ignored; used like a comment.

## The widget framework (`src/lelewidgets/`)

Strict 1:1 file pattern per widget:

```
lele<widget>.h      ← C++ class declaration (e.g. LeleLabel, LeleButton)
lele<widget>.cpp    ← C++ impl wrapping the underlying lv_obj_t
lele<widget>.py.cpp ← CPython binding (PyType, init, methods)
```

`LeleObject` is the base class. `LeleStyle` and `LeleEvent` are first-class
and also exposed to Python.

`LeleWidgetFactory::fromConfig(...)` parses the JSON, instantiates the
correct `LeleObject` subclass per key, and builds the tree.
`lele.loadConfig` on the Python side consumes that factory output.

So a single concept — say a `button` — exists in three places that must
stay aligned:

1. `"button":{...}` in the JSON schema
2. `LeleButton` in `lelebutton.h/.cpp`
3. `lele.Button` in `lelebutton.py.cpp`

When adding or modifying a widget, touch all three.

## Build

- `cmake -B build -S . && make -C build -j` (or `./build.sh`)
- Python `.so` modules are built and live at the repo root:
  `lele.cpython-3xx-*.so`, `mymodule.cpython-3xx-*.so`.
- Examples are run from inside the example directory so relative paths
  (`flags/...`, `res/...`, `<name>.json`) resolve.

## Pointers

- LVGL config: `lv_conf.h`, defaults in `lv_conf.defaults`.
- Backends selected via the `LV_USE_*` flags in `lv_conf.h`
  (fbdev, DRM/KMS, SDL2, Wayland, X11, OpenGLES, EVDEV).
- Display/input backend C sources: `src/lib/display_backends/`,
  `src/lib/indev_backends/`.
- Tests: `src/googletest/`, `src/lelewidgets/tests/`.
- Asset/font/i18n tooling: `src/image_builder/`, `src/lv_image_converter/`,
  `src/font/`, `src/tr/`.
