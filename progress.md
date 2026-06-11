# Progress — restaurant-1 demo

## Goal
Take the stock `restaurant-1` grid (61 numbered placeholder cells) and turn it
into a Panera-Bread-style touchscreen menu: scrollable grid of food cards,
detail page on tap, with a "+ Add to Bag" button that increments a bag
counter.

## Where we are now

`src/examples/restaurant-1/` is a working two-page stackview app:

- **Page 1 (grid):** green top bar with 7 round category chips (`What's New`,
  `You Pick Two`, `Must-Have`, `Mix & Match`, `Sandwiches`,
  **`Soups & Mac`** active in dark green, `Salads`) + a white `Q Search`
  pill on the right. Below: sub-tab row (`Featured` underlined, `Popular`,
  `New`) with the bag counter pill (`Bag  N`) on the right. Below: a
  scrollable grid of 25 white food cards, each with image, name, and
  `<type>  NNN Cal  $X.XX` subline.
- **Page 2 (detail):** slimmer green bar with `< Back` chip, title, and bag
  pill. Body: large image on the left half, right half has heading,
  description, price, and a green pill `+ Add to Bag` button. Tapping
  Add increments the bag counter and pops back to the grid.

## Files actively being worked on
- `src/examples/restaurant-1/restaurant-1.json` — scene graph (stackview +
  styles + 25 cards + detail template)
- `src/examples/restaurant-1/restaurant-1.py` — 25-item menu tuple, card-tap
  + order-tap event handlers, bag counter
- `src/examples/restaurant-1/foods/` — 25 PNG thumbnails (converted from the
  JPEGs in `src/res/foods/` because LVGL has `LV_USE_LIBJPEG_TURBO=0`)

## What landed this session (in order)
1. Converted 25 JPEGs in `src/res/foods/` → resized PNG thumbnails in
   `restaurant-1/foods/` via `ffmpeg` (needed `LD_LIBRARY_PATH=/usr/local/lib`
   because system `ffmpeg`/`convert` lookups are broken otherwise).
2. Replaced the 61 numbered `view→label` grid items with 25 image-bearing
   cards (initially `view`, later `button`).
3. Restructured into a `stackview` with two views (grid + detail) so card
   taps can `stackview.push` to a detail page.
4. Added 25-item menu data (name, description, price, image src) in Python.
   Card-click Python handler reads `event.target.id` ("card-7"), populates
   `/detail/img`, `/detail/name`, `/detail/desc`, `/detail/price` before the
   JSON event bubbles up to the stackview which runs the actual push.
5. Order-tap handler increments `bag_count`, updates both `/bag-grid` and
   `/bag-detail` labels, then declarative `stackview.pop` returns to grid.
6. Rewrote the green top bar to look like `panera-bread-app1.jpg`:
   7 column-flex category items (round chip + label), search pill.
7. Fixed a class-composition layout bug that made the topbar invisible
   (see "Key gotchas" below).

## Key gotchas hit (and worth remembering)
- **Class-tagged inline styles get dropped.** `getStylesByClass` in
  `leleobject.cpp:167-178` only includes a widget-local style if its
  `_class` is either empty OR matches the widget's class set. An inline
  style with `"class":"page"` on a widget whose class is `"stackview/view"`
  is silently ignored, taking its `flex`/`width`/`height` with it. Always
  leave inline styles unclassed; put `class:"X"` only on widget-level or on
  top-level reusable style definitions.
- **`view` widgets don't propagate action/args on click bubble.** For
  declarative `"event":{"action":"stackview.push","args":{...}}` to actually
  reach the stackview's event handler, the clickable widget has to be a
  `button` (`lelebutton.cpp:307-346` builds the action-carrying
  `LeleEvent`). Cards are `button`s for that reason.
- **`LeleStackView::updateBreadcrumbLabels` segfaults if the breadcrumb
  scaffolding is missing.** The stackview's top-level `view_header` needs
  ≥1 child `label`, and every `views:N/view` needs an inner `view_header`
  with a `name` — otherwise `*labels.begin()` or `getViewHeader(view)->name()`
  dereferences end()/nullptr. (`lelestackview.cpp:95, 105`.)
- **LVGL has no JPEG decoder in this build.** `lv_conf.h:913` ships with
  `LV_USE_LIBJPEG_TURBO 0` — only PNG (LODEPNG) and BMP work via LVGL's
  decoder path. The `/repos/utils/img_helper` wrapper (which calls into
  MagickCore) decodes the PNGs we're using.
- **`pkill -f 'lvglsim ...'` kills the wrapper bash.** The wrapper's argv
  contains the kill pattern, so `pkill -f` matches itself and the spawning
  shell dies with exit 144. Use `pgrep -f '/x86-build/bin/lvglsim' | xargs
  ps -o pid=,comm=` and filter to `comm==lvglsim` before killing.
- **`ffmpeg` / `convert` need `LD_LIBRARY_PATH=/usr/local/lib` on this box.**
  Their shared libs (libavdevice.so.61, libMagickCore-7.Q16HDRI.so.10)
  aren't on the default loader path.
- **Wayland display.** Running needs `WAYLAND_DISPLAY=/run/user/1000/wayland-1`
  and `XDG_RUNTIME_DIR=/run/user/1000` (matches `.vscode/launch.json`).

## How to run
```bash
PYTHONHOME=/repos/lv_port_linux/x86-build/bin/Python/ \
LD_LIBRARY_PATH=/usr/local/lib:/repos/lv_port_linux/x86-build/lib:/repos/lv_port_linux/x86-build/bin \
WAYLAND_DISPLAY=/run/user/1000/wayland-1 \
XDG_RUNTIME_DIR=/run/user/1000 \
/repos/lv_port_linux/x86-build/bin/lvglsim \
  /repos/lv_port_linux/src/examples/restaurant-1/restaurant-1.py
```
Logs land in `/tmp/restaurant-1.log` if redirected. App is viewed via the
container's RDP server (internal :3389 → external :4000); the user attaches
with `rdp-client.sh` from another machine.

## Current state
- App is running and stable (no segfaults, no Python tracebacks).
- Class-composition fix just landed — user is verifying that the green
  topbar with circles + search now actually renders.

## Open / next
- **Waiting on user visual confirmation** that the topbar now shows up
  correctly after the class fix.
- **Circles aren't clickable yet** — they're labels. To wire category
  filtering, swap each to a `button` with `id` like `cat-sandwiches` and
  add a Python handler.
- **Search is visual-only.** A real text input would use `LeleTextbox`.
- **No icons inside the circles** — only initials (`NEW`, `2`, `MHM`,
  `M&M`, `SW`, `S&M`, `SAL`) because Ubuntu-B doesn't ship Panera's icon
  glyphs. Could replace each with a small PNG (`img` inside the cat
  view) if we draw / source icons.
- **Bag counter** is in the subtabs row, not floating over the grid like
  the picture's circular `0` chip. Easy to relocate / restyle.

## Reference paths
- LVGL config: `lv_conf.h`
- Widget framework: `src/lelewidgets/` (`lele<widget>.{h,cpp,py.cpp}` triple)
- JSON→C++ dispatch: `src/lelewidgets/lelewidgetfactory.cpp:177-244`
- Event bubble model: `src/lelewidgets/leleobject.cpp:660-672`
- Stackview push/pop: `src/lelewidgets/lelestackview.cpp:208-234`
- Python binding entry: `src/python/python_binding.cpp`
