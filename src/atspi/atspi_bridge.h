#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include "atspi_constants.h"
#include "atspi_dbus.h"
#include "accessible_node.h"

struct _lv_obj_t;
typedef struct _lv_obj_t lv_obj_t;

// Top-level singleton that:
// 1. Connects to AT-SPI D-Bus and registers the application
// 2. Maintains the accessible tree (synced with LVGL widget tree)
// 3. Bridges LVGL events → AT-SPI D-Bus signals
// 4. Dispatches incoming AT-SPI method calls
struct AtspiBridge {
    // Initialize bridge. Returns true if D-Bus session bus is available
    // and AT-SPI registration succeeded (or registry is reachable).
    static bool init();

    // Shutdown: unregister, disconnect.
    static void shutdown();

    // Re-scan the entire LVGL object tree and rebuild accessible nodes.
    // Called on init and when the scene graph changes.
    static void rebuildTree();

    // Get or create the AccessibleNode for a given lv_obj_t*.
    // Returns existing node if already tracked, creates one if not.
    static AccessibleNode *nodeForLvglObj(lv_obj_t *obj);

    // Remove a node from the tree when its LVGL object is deleted.
    static void removeNode(lv_obj_t *obj);

    // --- Event emission helpers ---
    // Call these from LVGL event callbacks.

    // Emit object:state-changed when a widget's state changes
    static void emitStateChanged(lv_obj_t *obj, uint32_t state,
                                 bool enabled, const char *detail = "");

    // Emit object:property-change for name, description, etc.
    static void emitPropertyChanged(lv_obj_t *obj, const char *property,
                                    const char *value);

    // Emit object:children-changed when widgets are added/removed
    static void emitChildrenChanged(lv_obj_t *parent, int index,
                                    bool added, lv_obj_t *child);

    // Emit focus: event (special AT-SPI focus signal)
    static void emitFocus(lv_obj_t *obj);

    // Emit object:visible-data-changed
    static void emitVisibleDataChanged(lv_obj_t *obj);

    // Emit window:activate / window:deactivate
    static void emitWindowActivate(lv_obj_t *win, bool active);

    // --- Public accessors ---
    static bool isInitialized() { return _initialized; }
    static const std::string &appBusName() { return _app_bus_name; }
    static uint32_t appId() { return _app_id; }

private:
    static bool _initialized;
    static std::string _app_bus_name;
    static uint32_t _app_id;

    // All tracked nodes (indexed by lv_obj_t* address)
    static std::vector<AccessibleNode *> _all_nodes;

    // D-Bus method dispatch for all AT-SPI interfaces
    static bool methodHandler(const char *path,
                              const char *interface,
                              const char *method,
                              DBusMessage *args,
                              DBusMessage **reply);

    // Recursive tree builder
    static AccessibleNode *buildNode(lv_obj_t *lv_obj, AccessibleNode *parent);

    // Register on the AT-SPI bus (Socket.Embed handshake)
    static bool registerWithRegistry();

    // Register each LVGL event we want to bridge
    static void installLvglEventHooks();

    // LVGL event callbacks (static, registered on every top-level screen)
    static void onLvglEventFocus(lv_obj_t *obj);
    static void onLvglEventValueChanged(lv_obj_t *obj);
    static void onLvglEventChildChanged(lv_obj_t *obj);
    static void onLvglEventDelete(lv_obj_t *obj);
};
