#pragma once

#include <src/drivers/wayland/lv_wayland_private.h>
#include <src/lib/backends.h>

class GraphicsBackend {
public:
    static GraphicsBackend &getInstance();
    bool load();
    bool handleEvents() const;
    backend_t *getBackend() const;
    lv_point_t getTouchPoint() const;
protected:
    GraphicsBackend() = default;
    backend_t *_backend;
};