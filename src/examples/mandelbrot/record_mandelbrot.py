import os
import sys
import lele
import time

status_label = None
play_btn = None

has_cuda = False
playing = False
speed_factor = 1.03
center_x = -0.5
center_y = 0.0
zoom = 1.0
frame_count = 0

def updateStatus(text):
    global status_label
    if status_label:
        status_label.setText(text)

def update_image():
    global center_x, center_y, zoom
    real_range = 3.0 / zoom
    imag_range = 2.0 / zoom
    real_min = center_x - real_range / 2
    real_max = center_x + real_range / 2
    imag_min = center_y - imag_range / 2
    imag_max = center_y + imag_range / 2
    return lele.updateMandelbrotImage("/mandelbrot/image", 1000, 4.0,
                                      real_min, real_max, imag_min, imag_max)

def on_play(event):
    global playing
    playing = True
    updateStatus("Zooming...")

has_cuda = lele.hasCuda()
if has_cuda:
    lele.generateMandelbrot("res/mandelbrot.png", 800, 600)

lele.loadConfig("mandelbrot.json")
status_label = lele.getObjectById("/mandelbrot/status")
play_btn = lele.getObjectById("/mandelbrot/play")
if play_btn:
    play_btn.addEventHandler(on_play)

updateStatus("Auto-playing...")
last_frame_time = time.time()
cap_idx = 0
auto_played = False
cap_start = 0

while lele.handleEvents():
    now = time.time()
    if not auto_played and now - 0.0 > 0.5:
        if play_btn:
            play_btn.click()
        auto_played = True
        cap_start = now
        last_frame_time = now
    if playing:
        dt = now - last_frame_time
        if dt >= 0.033:
            last_frame_time = now
            zoom *= speed_factor
            frame_count += 1
            update_image()
        if cap_idx < 60 and now - cap_start > cap_idx * 0.5:
            lele.dumpScreenshot()
            src = f"/tmp/screenshot-{cap_idx % 100}.png"
            dst = f"/tmp/mandelbrot_frames/frame-{cap_idx:04d}.png"
            if os.path.exists(src):
                os.rename(src, dst)
                cap_idx += 1
                updateStatus(f"Captured {cap_idx}/60")
                if cap_idx >= 60:
                    updateStatus("Captured 60 frames")
                    break
    time.sleep(0.001)

updateStatus("Done")
