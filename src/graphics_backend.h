#pragma once

#include <src/drivers/wayland/lv_wayland_private.h>
#include <src/lib/backends.h>

class GraphicsBackend {
public:
    bool load();
    bool handleEvents();
protected:
    backend_t *_backend;
};