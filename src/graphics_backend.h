#pragma once

#include <lvgl/lvgl.h>
#include <src/lib/backends.h>
#if LV_USE_WAYLAND
#include <src/drivers/wayland/lv_wayland_private.h>
#endif

class GraphicsBackend {
public:
    static GraphicsBackend &getInstance();
    bool load(int screen_width, int screen_height);
    bool handleEvents() const;
    backend_t *getBackend() const;
    lv_point_t getTouchPoint(lv_obj_t *obj = nullptr) const;
    void dumpScreenshot() const;
protected:
    GraphicsBackend() = default;
    backend_t *_backend;
};