import os
import sys
import lele
import time

status_label = None
image_widget = None
play_btn = None
pause_btn = None
reset_btn = None
speed_slider = None
palette_dropdown = None

has_cuda = False
playing = False
paused = False
speed_factor = 1.03
center_x = -0.7269
center_y = 0.1889
zoom = 1.0
frame_count = 0
color_scheme = 0

palette_map = {
    "Classic Blue": 0,
    "Fire Heat": 1,
    "Rainbow": 2,
    "Electric": 3,
}

def updateStatus(text):
    global status_label
    if status_label:
        status_label.setText(text)

def update_image():
    global center_x, center_y, zoom, color_scheme
    real_range = 3.0 / zoom
    imag_range = 2.0 / zoom
    real_min = center_x - real_range / 2
    real_max = center_x + real_range / 2
    imag_min = center_y - imag_range / 2
    imag_max = center_y + imag_range / 2
    ok = lele.updateMandelbrotImage("/mandelbrot/image", 1000, 4.0,
                                    real_min, real_max, imag_min, imag_max,
                                    color_scheme)
    if not ok:
        updateStatus("Failed to render frame")
    return ok

def on_play(event):
    global playing, paused
    playing = True
    paused = False
    updateStatus("Zooming...")

def on_pause(event):
    global paused
    if playing:
        paused = not paused
        updateStatus("Paused" if paused else "Zooming...")

def on_reset(event):
    global playing, paused, center_x, center_y, zoom, frame_count
    playing = False
    paused = False
    center_x = -0.7269
    center_y = 0.1889
    zoom = 1.0
    frame_count = 0
    updateStatus("Reset")
    update_image()

def on_nudge(dx, dy):
    global center_x, center_y
    shift = 0.2 / zoom
    center_x += dx * shift
    center_y += dy * shift
    if playing:
        updateStatus(f"Center: ({center_x:.4f}, {center_y:.4f}) zoom: {zoom:.1f}x")
    else:
        update_image()

def on_left(event):
    on_nudge(-1, 0)

def on_right(event):
    on_nudge(1, 0)

def on_up(event):
    on_nudge(0, -1)

def on_down(event):
    on_nudge(0, 1)

def on_speed_change(event):
    global speed_factor
    val = event.value
    if val <= 0:
        return  # PRESSED/RELEASED events pass value=0, skip them
    speed_factor = 1.0 + val * 0.005

def on_palette_change(event):
    global color_scheme
    if palette_dropdown:
        sel = palette_dropdown.getSelectedItem()
        if sel in palette_map:
            color_scheme = palette_map[sel]
            updateStatus(f"Palette: {sel}")
            update_image()

has_cuda = lele.hasCuda()

res = lele.loadConfig("mandelbrot.json")
if not res:
    sys.exit(1)

status_label = lele.getObjectById("/mandelbrot/status")
image_widget = lele.getObjectById("/mandelbrot/image")
play_btn = lele.getObjectById("/mandelbrot/play")
pause_btn = lele.getObjectById("/mandelbrot/pause")
reset_btn = lele.getObjectById("/mandelbrot/reset")
left_btn = lele.getObjectById("/mandelbrot/left")
right_btn = lele.getObjectById("/mandelbrot/right")
up_btn = lele.getObjectById("/mandelbrot/up")
down_btn = lele.getObjectById("/mandelbrot/down")
speed_slider = lele.getObjectById("/mandelbrot/speed_slider")
palette_dropdown = lele.getObjectById("/mandelbrot/palette")

if play_btn and pause_btn and reset_btn and speed_slider:
    play_btn.addEventHandler(on_play)
    pause_btn.addEventHandler(on_pause)
    reset_btn.addEventHandler(on_reset)
    left_btn.addEventHandler(on_left)
    right_btn.addEventHandler(on_right)
    up_btn.addEventHandler(on_up)
    down_btn.addEventHandler(on_down)
    speed_slider.addEventHandler(on_speed_change)
else:
    updateStatus("Warning: some controls not found")
    print("Warning: some controls not found", file=sys.stderr)

if palette_dropdown:
    palette_dropdown.onValueChanged(on_palette_change)

if has_cuda:
    updateStatus("Generating Mandelbrot set with CUDA...")
    img_path = os.path.join(os.getcwd(), "res/mandelbrot.png")
    ok = lele.generateMandelbrot(img_path, 800, 600)
    if ok:
        updateStatus("Ready - press Play to zoom")
    else:
        updateStatus("Mandelbrot generation failed")
else:
    updateStatus("GPU is required - this app needs CUDA to run")

print("ENTRY: Entering event loop", flush=True)
last_frame_time = time.time()
screenshot_count = 0
while lele.handleEvents():
    if playing and not paused:
        now = time.time()
        dt = now - last_frame_time
        if dt >= 0.033:
            last_frame_time = now
            zoom *= speed_factor
            if zoom > 100000.0:
                zoom = 100000.0
                updateStatus(f"Max zoom reached ({zoom:.0f}x)")
            frame_count += 1
            if frame_count % 5 == 0:
                updateStatus(f"Zoom: {zoom:.1f}x  Center: ({center_x:.4f}, {center_y:.4f})")
            ok = update_image()
            if not ok:
                playing = False
                updateStatus("Render failed - stopped")

    if screenshot_count < 2 and time.time() > 5.0 + screenshot_count * 5.0:
        lele.dumpScreenshot()
        print(f"Screenshot {screenshot_count + 1} taken", flush=True)
        screenshot_count += 1

    time.sleep(0.001)
