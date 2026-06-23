#include "atspi_bridge.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <map>

#include <dbus/dbus.h>

// Static members
bool AtspiBridge::_initialized = false;
std::string AtspiBridge::_app_bus_name;
uint32_t AtspiBridge::_app_id = 0;
std::vector<AccessibleNode *> AtspiBridge::_all_nodes;

// For fast lookup: lv_obj_t* → AccessibleNode*
// Stored as user_data (LVGL user data pointer) on each tracked object.
// We also keep a map for safety.
static std::map<lv_obj_t *, AccessibleNode *> s_node_map;

// Registered object path: the root and all descendant object paths
// are handled by a single handler that looks up the node.
static const char *ROOT_PATH = ATSPI_DBUS_PATH_ROOT;

// ─── Initialization ────────────────────────────────────────────

bool AtspiBridge::init() {
    if (_initialized) return true;

    if (!AtspiDbus::connect()) {
        fprintf(stderr, "[atspi] Could not connect to D-Bus\n");
        return false;
    }

    _app_bus_name = dbus_bus_get_unique_name(AtspiDbus::conn());
    printf("[atspi] Unique bus name: %s\n", _app_bus_name.c_str());

    // Register the root path handler for all AT-SPI paths
    // (All accessible object paths are under /org/a11y/atspi/accessible/)
    AtspiDbus::registerObject(ROOT_PATH, methodHandler);
    AtspiDbus::registerObject(ATSPI_DBUS_PATH_NULL, methodHandler);
    AtspiDbus::registerObject(ATSPI_DBUS_PATH_DEC, methodHandler);

    // Walk the existing LVGL widget tree
    rebuildTree();

    // Install event hooks to bridge LVGL events → AT-SPI signals
    installLvglEventHooks();

    // Register with the AT-SPI registry daemon
    if (!registerWithRegistry()) {
        fprintf(stderr, "[atspi] Warning: AT-SPI registry not available "
                "(no accessibility tools will see this app)\n");
        // Continue — the app is still accessible to ATs that connect later
    }

    _initialized = true;
    printf("[atspi] Bridge initialized\n");
    return true;
}

void AtspiBridge::shutdown() {
    if (!_initialized) return;

    // Unregister from AT-SPI (optional — registry detects disconnect)
    _initialized = false;

    // Free all accessible nodes
    for (auto *node : _all_nodes) {
        delete node;
    }
    _all_nodes.clear();
    s_node_map.clear();

    AtspiDbus::disconnect();
}

// ─── Tree management ───────────────────────────────────────────

AccessibleNode *AtspiBridge::nodeForLvglObj(lv_obj_t *obj) {
    auto it = s_node_map.find(obj);
    if (it != s_node_map.end()) {
        return it->second;
    }
    return nullptr;
}

AccessibleNode *AtspiBridge::buildNode(lv_obj_t *lv_obj, AccessibleNode *parent) {
    auto *node = new AccessibleNode(lv_obj);
    node->parent = parent;

    // Assign object path based on a counter (simplified; hash-based would be better)
    static uint32_t s_node_counter = 0;
    if (parent && parent->path == ATSPI_DBUS_PATH_ROOT) {
        // Direct child of root: /org/a11y/atspi/accessible/dec/{N}
        node->path = std::string(ATSPI_DBUS_PATH_DEC) + "/" + std::to_string(++s_node_counter);
    } else if (parent) {
        node->path = parent->path + "/" + std::to_string(++s_node_counter);
    } else {
        node->path = ATSPI_DBUS_PATH_ROOT;
    }

    // Determine role from widget class
    const char *class_name = widgetClassName(lv_obj);
    node->role = lvglClassToAtspiRole(class_name);

    // Register the object path for this node
    AtspiDbus::registerObject(node->path.c_str(), methodHandler);

    // Store in maps
    _all_nodes.push_back(node);
    s_node_map[lv_obj] = node;

    // Recurse into children
    uint32_t child_count = lv_obj_get_child_count(lv_obj);
    for (uint32_t i = 0; i < child_count; i++) {
        lv_obj_t *child = lv_obj_get_child(lv_obj, i);
        if (child) {
            auto *child_node = buildNode(child, node);
            node->children.push_back(child_node);
        }
    }

    return node;
}

void AtspiBridge::rebuildTree() {
    // Clear existing tree
    for (auto *node : _all_nodes) {
        delete node;
    }
    _all_nodes.clear();
    s_node_map.clear();

    // Walk all displays and their screens
    lv_display_t *disp = lv_display_get_next(nullptr);
    while (disp) {
        lv_obj_t *screen = lv_display_get_screen_active(disp);
        if (screen) {
            buildNode(screen, nullptr);
        }

        // Also walk layer-top and layer-sys
        lv_obj_t *layer_top = lv_display_get_layer_top(disp);
        if (layer_top) {
            auto *node = buildNode(layer_top, nodeForLvglObj(screen));
            if (node && _all_nodes.size() > 1) {
                _all_nodes[0]->children.push_back(node);
            }
        }

        disp = lv_display_get_next(disp);
    }
}

void AtspiBridge::removeNode(lv_obj_t *obj) {
    auto it = s_node_map.find(obj);
    if (it == s_node_map.end()) return;

    AccessibleNode *node = it->second;

    // Remove from parent's children list
    if (node->parent) {
        auto &siblings = node->parent->children;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), node),
                        siblings.end());
    }

    // Remove children recursively
    for (auto *child : node->children) {
        s_node_map.erase(child->lv_obj);
        _all_nodes.erase(std::remove(_all_nodes.begin(), _all_nodes.end(), child),
                          _all_nodes.end());
        delete child;
    }

    s_node_map.erase(obj);
    _all_nodes.erase(std::remove(_all_nodes.begin(), _all_nodes.end(), node),
                      _all_nodes.end());
    delete node;
}

// ─── AT-SPI Registration ───────────────────────────────────────

bool AtspiBridge::registerWithRegistry() {
    DBusError err;
    dbus_error_init(&err);

    // Check if registry is on the bus
    DBusMessage *ping = dbus_message_new_method_call(
        ATSPI_DBUS_NAME_REGISTRY,
        ATSPI_DBUS_PATH_REGISTRY,
        "org.freedesktop.DBus.Introspectable",
        "Introspect");

    if (!ping) return false;

    DBusMessage *reply = dbus_connection_send_with_reply_and_block(
        AtspiDbus::conn(), ping, 2000, &err);
    dbus_message_unref(ping);

    if (!reply) {
        // Registry not available
        dbus_error_free(&err);
        return false;
    }
    dbus_message_unref(reply);

    // Call Socket.Embed to register the application
    // The "plug" is a (so) tuple: (bus_unique_name, /org/a11y/atspi/accessible/root)
    DBusMessage *embed = dbus_message_new_method_call(
        ATSPI_DBUS_NAME_REGISTRY,
        ATSPI_DBUS_PATH_REGISTRY,
        ATSPI_INTERFACE_SOCKET,
        "Embed");

    if (!embed) return false;

    AtspiDbus::appendRef(embed, _app_bus_name.c_str(), ATSPI_DBUS_PATH_ROOT);

    dbus_error_init(&err);
    DBusMessage *embed_reply = dbus_connection_send_with_reply_and_block(
        AtspiDbus::conn(), embed, -1, &err);
    dbus_message_unref(embed);

    if (!embed_reply) {
        fprintf(stderr, "[atspi] Socket.Embed failed: %s\n", err.message);
        dbus_error_free(&err);
        return false;
    }

    // The reply is a (so) tuple (the socket reference)
    // We don't need the socket for anything currently, but we could store it.
    dbus_message_unref(embed_reply);

    printf("[atspi] Registered with AT-SPI registry\n");
    return true;
}

// ─── LVGL event hooks ──────────────────────────────────────────

// Generic LVGL event callback that dispatches to AT-SPI
static void lvgl_atspi_event_cb(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);
    uint32_t code = lv_event_get_code(e);

    switch (code) {
        case LV_EVENT_FOCUSED:
            AtspiBridge::emitFocus(obj);
            break;
        case LV_EVENT_DEFOCUSED:
            AtspiBridge::emitStateChanged(obj, STATE_FOCUSED, false);
            break;
        case LV_EVENT_VALUE_CHANGED:
            AtspiBridge::emitPropertyChanged(obj, "accessible-value", "");
            break;
        case LV_EVENT_CHILD_CHANGED:
            // The accessible tree needs a partial rebuild around this object
            // For simplicity, we emit children-changed on the parent
            {
                lv_obj_t *par = lv_obj_get_parent(obj);
                if (par) {
                    AtspiBridge::emitChildrenChanged(par, 0, true, obj);
                }
            }
            break;
        case LV_EVENT_DELETE:
            AtspiBridge::removeNode(obj);
            break;
        default:
            break;
    }
}

static bool s_hooks_installed = false;

// Install hooks on all existing and future display screens
static void screen_load_event_cb(lv_event_t *e) {
    lv_obj_t *screen = lv_event_get_target(e);
    if (!screen) return;

    // Rebuild accessible tree when screen loads
    // Check if this is a new screen
    (void)screen;
    AtspiBridge::rebuildTree();
}

void AtspiBridge::installLvglEventHooks() {
    if (s_hooks_installed) return;

    // Register a global event hook on the group for focus tracking
    lv_group_t *group = lv_group_get_default();
    if (!group) {
        group = lv_group_create();
        lv_group_set_default(group);
    }

    // Install event callbacks on all existing screens
    lv_display_t *disp = lv_display_get_next(nullptr);
    while (disp) {
        lv_obj_t *screen = lv_display_get_screen_active(disp);
        if (screen) {
            lv_obj_add_event(screen, lvgl_atspi_event_cb,
                              LV_EVENT_FOCUSED, nullptr);
            lv_obj_add_event(screen, lvgl_atspi_event_cb,
                              LV_EVENT_DEFOCUSED, nullptr);
            lv_obj_add_event(screen, lvgl_atspi_event_cb,
                              LV_EVENT_VALUE_CHANGED, nullptr);
            lv_obj_add_event(screen, lvgl_atspi_event_cb,
                              LV_EVENT_CHILD_CHANGED, nullptr);
            lv_obj_add_event(screen, lvgl_atspi_event_cb,
                              LV_EVENT_DELETE, nullptr);
            lv_obj_add_event(screen, screen_load_event_cb,
                              LV_EVENT_SCREEN_LOADED, nullptr);
        }
        disp = lv_display_get_next(disp);
    }

    s_hooks_installed = true;
}

// ─── Event emission ────────────────────────────────────────────

void AtspiBridge::emitStateChanged(lv_obj_t *obj, uint32_t state,
                                    bool enabled, const char *detail) {
    auto *node = nodeForLvglObj(obj);
    if (!node) return;

    // Build the event signal body:
    //   source s, detail1 i, detail2 i, data v, properties a{sv}
    DBusMessage *signal = dbus_message_new_signal(
        node->path.c_str(),
        ATSPI_INTERFACE_EVENT_OBJECT,
        "StateChanged");

    if (!signal) return;

    DBusMessageIter iter;
    dbus_message_iter_init_append(signal, &iter);

    // source (s) — app bus name
    const char *src = _app_bus_name.c_str();
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &src);

    // detail1 (i) — the state enum value
    int32_t d1 = (int32_t)state;
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &d1);

    // detail2 (i) — 1 if enabled, 0 if disabled
    int32_t d2 = enabled ? 1 : 0;
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &d2);

    // data (v) — typically empty for state-changed
    DBusMessageIter variant_iter;
    dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT,
                                      DBUS_TYPE_STRING_AS_STRING, &variant_iter);
    const char *empty = "";
    dbus_message_iter_append_basic(&variant_iter, DBUS_TYPE_STRING, &empty);
    dbus_message_iter_close_container(&iter, &variant_iter);

    // properties (a{sv}) — empty
    DBusMessageIter dict_iter;
    dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
                                      DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
                                      DBUS_TYPE_STRING_AS_STRING
                                      DBUS_TYPE_VARIANT_AS_STRING
                                      DBUS_DICT_ENTRY_END_CHAR_AS_STRING,
                                      &dict_iter);
    dbus_message_iter_close_container(&iter, &dict_iter);

    AtspiDbus::emitSignal(node->path.c_str(), ATSPI_INTERFACE_EVENT_OBJECT,
                           "StateChanged", signal);
    dbus_message_unref(signal);
}

void AtspiBridge::emitPropertyChanged(lv_obj_t *obj, const char *property,
                                       const char *value) {
    auto *node = nodeForLvglObj(obj);
    if (!node) return;

    DBusMessage *signal = dbus_message_new_signal(
        node->path.c_str(),
        ATSPI_INTERFACE_EVENT_OBJECT,
        "PropertyChange");

    if (!signal) return;

    DBusMessageIter iter;
    dbus_message_iter_init_append(signal, &iter);

    const char *src = _app_bus_name.c_str();
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &src);

    // detail1 (i) — 0 for property-change, detail2 (i) — 0
    int32_t zero = 0;
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &zero);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &zero);

    // data (v) — the property name
    DBusMessageIter variant_iter;
    dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT,
                                      DBUS_TYPE_STRING_AS_STRING, &variant_iter);
    dbus_message_iter_append_basic(&variant_iter, DBUS_TYPE_STRING, &property);
    dbus_message_iter_close_container(&iter, &variant_iter);

    // properties (a{sv}) — empty
    DBusMessageIter dict_iter;
    dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
                                      DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
                                      DBUS_TYPE_STRING_AS_STRING
                                      DBUS_TYPE_VARIANT_AS_STRING
                                      DBUS_DICT_ENTRY_END_CHAR_AS_STRING,
                                      &dict_iter);
    dbus_message_iter_close_container(&iter, &dict_iter);

    AtspiDbus::emitSignal(node->path.c_str(), ATSPI_INTERFACE_EVENT_OBJECT,
                           "PropertyChange", signal);
    dbus_message_unref(signal);
}

void AtspiBridge::emitChildrenChanged(lv_obj_t *parent, int index,
                                       bool added, lv_obj_t *child) {
    auto *node = nodeForLvglObj(parent);
    if (!node) return;

    DBusMessage *signal = dbus_message_new_signal(
        node->path.c_str(),
        ATSPI_INTERFACE_EVENT_OBJECT,
        "ChildrenChanged");

    if (!signal) return;

    DBusMessageIter iter;
    dbus_message_iter_init_append(signal, &iter);

    const char *src = _app_bus_name.c_str();
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &src);

    // detail1 (i) — index in parent
    int32_t d1 = index;
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &d1);

    // detail2 (i) — 1 if added, 0 if removed
    int32_t d2 = added ? 1 : 0;
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &d2);

    // data (v) — the (so) reference of the child
    DBusMessageIter variant_iter;
    dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT,
                                      "(so)", &variant_iter);
    auto *child_node = nodeForLvglObj(child);
    AtspiDbus::appendRef(&variant_iter, _app_bus_name.c_str(),
                          child_node ? child_node->path.c_str() : ATSPI_DBUS_PATH_NULL);
    dbus_message_iter_close_container(&iter, &variant_iter);

    // properties (a{sv}) — empty
    DBusMessageIter dict_iter;
    dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
                                      DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
                                      DBUS_TYPE_STRING_AS_STRING
                                      DBUS_TYPE_VARIANT_AS_STRING
                                      DBUS_DICT_ENTRY_END_CHAR_AS_STRING,
                                      &dict_iter);
    dbus_message_iter_close_container(&iter, &dict_iter);

    AtspiDbus::emitSignal(node->path.c_str(), ATSPI_INTERFACE_EVENT_OBJECT,
                           "ChildrenChanged", signal);
    dbus_message_unref(signal);
}

void AtspiBridge::emitFocus(lv_obj_t *obj) {
    auto *node = nodeForLvglObj(obj);
    if (!node) return;

    // Emit two signals:
    // 1. object:state-changed(focused, 1) on the object
    emitStateChanged(obj, STATE_FOCUSED, true);

    // 2. focus: signal on the application root
    DBusMessage *signal = dbus_message_new_signal(
        ATSPI_DBUS_PATH_ROOT,
        ATSPI_INTERFACE_EVENT_FOCUS,
        "Focus");

    if (!signal) return;

    DBusMessageIter iter;
    dbus_message_iter_init_append(signal, &iter);

    const char *src = _app_bus_name.c_str();
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &src);

    int32_t d1 = 0, d2 = 0;
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &d1);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &d2);

    // data (v) — the (so) reference of the focused object
    DBusMessageIter variant_iter;
    dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT,
                                      "(so)", &variant_iter);
    AtspiDbus::appendRef(&variant_iter, _app_bus_name.c_str(),
                          node->path.c_str());
    dbus_message_iter_close_container(&iter, &variant_iter);

    // properties — empty
    DBusMessageIter dict_iter;
    dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
                                      DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
                                      DBUS_TYPE_STRING_AS_STRING
                                      DBUS_TYPE_VARIANT_AS_STRING
                                      DBUS_DICT_ENTRY_END_CHAR_AS_STRING,
                                      &dict_iter);
    dbus_message_iter_close_container(&iter, &dict_iter);

    AtspiDbus::emitSignal(ATSPI_DBUS_PATH_ROOT, ATSPI_INTERFACE_EVENT_FOCUS,
                           "Focus", signal);
    dbus_message_unref(signal);
}

void AtspiBridge::emitVisibleDataChanged(lv_obj_t *obj) {
    auto *node = nodeForLvglObj(obj);
    if (!node) return;

    DBusMessage *signal = dbus_message_new_signal(
        node->path.c_str(),
        ATSPI_INTERFACE_EVENT_OBJECT,
        "VisibleDataChanged");

    if (!signal) return;

    DBusMessageIter iter;
    dbus_message_iter_init_append(signal, &iter);

    const char *src = _app_bus_name.c_str();
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &src);

    int32_t zero = 0;
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &zero);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &zero);

    DBusMessageIter variant_iter;
    dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT,
                                      DBUS_TYPE_STRING_AS_STRING, &variant_iter);
    const char *empty = "";
    dbus_message_iter_append_basic(&variant_iter, DBUS_TYPE_STRING, &empty);
    dbus_message_iter_close_container(&iter, &variant_iter);

    DBusMessageIter dict_iter;
    dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
                                      DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
                                      DBUS_TYPE_STRING_AS_STRING
                                      DBUS_TYPE_VARIANT_AS_STRING
                                      DBUS_DICT_ENTRY_END_CHAR_AS_STRING,
                                      &dict_iter);
    dbus_message_iter_close_container(&iter, &dict_iter);

    AtspiDbus::emitSignal(node->path.c_str(), ATSPI_INTERFACE_EVENT_OBJECT,
                           "VisibleDataChanged", signal);
    dbus_message_unref(signal);
}

void AtspiBridge::emitWindowActivate(lv_obj_t *win, bool active) {
    auto *node = nodeForLvglObj(win);
    if (!node) return;

    const char *signal_name = active ? "Activate" : "Deactivate";

    DBusMessage *signal = dbus_message_new_signal(
        node->path.c_str(),
        ATSPI_INTERFACE_EVENT_WINDOW,
        signal_name);

    if (!signal) return;

    DBusMessageIter iter;
    dbus_message_iter_init_append(signal, &iter);

    const char *src = _app_bus_name.c_str();
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &src);

    int32_t zero = 0;
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &zero);
    dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &zero);

    DBusMessageIter variant_iter;
    dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT,
                                      DBUS_TYPE_STRING_AS_STRING, &variant_iter);
    const char *empty = "";
    dbus_message_iter_append_basic(&variant_iter, DBUS_TYPE_STRING, &empty);
    dbus_message_iter_close_container(&iter, &variant_iter);

    DBusMessageIter dict_iter;
    dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
                                      DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
                                      DBUS_TYPE_STRING_AS_STRING
                                      DBUS_TYPE_VARIANT_AS_STRING
                                      DBUS_DICT_ENTRY_END_CHAR_AS_STRING,
                                      &dict_iter);
    dbus_message_iter_close_container(&iter, &dict_iter);

    AtspiDbus::emitSignal(node->path.c_str(), ATSPI_INTERFACE_EVENT_WINDOW,
                           signal_name, signal);
    dbus_message_unref(signal);
}

// ─── D-Bus method dispatch ─────────────────────────────────────

// Helper: read a string from a message iterator
static bool iterGetString(DBusMessageIter *iter, const char **out) {
    if (dbus_message_iter_get_arg_type(iter) != DBUS_TYPE_STRING) return false;
    dbus_message_iter_get_basic(iter, out);
    return true;
}

// Helper: read a (so) tuple from a message iterator
static bool iterGetRef(DBusMessageIter *iter,
                        const char **bus_name, const char **path) {
    if (dbus_message_iter_get_arg_type(iter) != DBUS_TYPE_STRUCT) return false;
    DBusMessageIter sub;
    dbus_message_iter_recurse(iter, &sub);
    if (dbus_message_iter_get_arg_type(&sub) != DBUS_TYPE_STRING) return false;
    dbus_message_iter_get_basic(&sub, bus_name);
    if (!dbus_message_iter_next(&sub)) return false;
    if (dbus_message_iter_get_arg_type(&sub) != DBUS_TYPE_OBJECT_PATH) return false;
    dbus_message_iter_get_basic(&sub, path);
    return true;
}

bool AtspiBridge::methodHandler(const char *path,
                                 const char *interface,
                                 const char *method,
                                 DBusMessage *args,
                                 DBusMessage **reply) {
    // Look up the node by path (brute-force for scaffold; use a map in production)
    AccessibleNode *node = nullptr;
    if (strcmp(path, ATSPI_DBUS_PATH_ROOT) == 0) {
        // Root node
        if (!_all_nodes.empty()) node = _all_nodes[0];
    } else {
        for (auto *n : _all_nodes) {
            if (n->path == path) { node = n; break; }
        }
    }

    // Properties
    if (strcmp(interface, DBUS_INTERFACE_PROPERTIES) == 0) {
        if (strcmp(method, "Get") == 0) {
            DBusMessageIter iter;
            dbus_message_iter_init(args, &iter);

            const char *iface_name = nullptr, *prop_name = nullptr;
            if (!iterGetString(&iter, &iface_name)) return false;
            if (!dbus_message_iter_next(&iter)) return false;
            if (!iterGetString(&iter, &prop_name)) return false;

            // Dispatch property reads
            *reply = dbus_message_new_method_return(args);
            DBusMessageIter reply_iter;
            dbus_message_iter_init_append(*reply, &reply_iter);

            bool handled = false;

            if (strcmp(iface_name, ATSPI_INTERFACE_ACCESSIBLE) == 0) {
                if (strcmp(prop_name, "Name") == 0) {
                    std::string name = node ? node->getName() : "root";
                    DBusMessageIter variant;
                    dbus_message_iter_open_container(&reply_iter,
                        DBUS_TYPE_VARIANT, DBUS_TYPE_STRING_AS_STRING, &variant);
                    const char *cstr = name.c_str();
                    dbus_message_iter_append_basic(&variant, DBUS_TYPE_STRING, &cstr);
                    dbus_message_iter_close_container(&reply_iter, &variant);
                    handled = true;
                }
                else if (strcmp(prop_name, "ChildCount") == 0) {
                    int32_t cnt = node ? node->getChildCount() : 0;
                    DBusMessageIter variant;
                    dbus_message_iter_open_container(&reply_iter,
                        DBUS_TYPE_VARIANT, DBUS_TYPE_INT32_AS_STRING, &variant);
                    dbus_message_iter_append_basic(&variant, DBUS_TYPE_INT32, &cnt);
                    dbus_message_iter_close_container(&reply_iter, &variant);
                    handled = true;
                }
                else if (strcmp(prop_name, "Parent") == 0) {
                    DBusMessageIter variant;
                    dbus_message_iter_open_container(&reply_iter,
                        DBUS_TYPE_VARIANT, "(so)", &variant);
                    if (node) {
                        auto p = node->getParent();
                        AtspiDbus::appendRef(&variant, _app_bus_name.c_str(),
                                              p.second.c_str());
                    } else {
                        AtspiDbus::appendRef(&variant, _app_bus_name.c_str(),
                                              ATSPI_DBUS_PATH_NULL);
                    }
                    dbus_message_iter_close_container(&reply_iter, &variant);
                    handled = true;
                }
            }
            else if (strcmp(iface_name, ATSPI_INTERFACE_APPLICATION) == 0) {
                if (strcmp(prop_name, "ToolkitName") == 0) {
                    const char *val = "LVGL";
                    DBusMessageIter variant;
                    dbus_message_iter_open_container(&reply_iter,
                        DBUS_TYPE_VARIANT, DBUS_TYPE_STRING_AS_STRING, &variant);
                    dbus_message_iter_append_basic(&variant, DBUS_TYPE_STRING, &val);
                    dbus_message_iter_close_container(&reply_iter, &variant);
                    handled = true;
                }
                else if (strcmp(prop_name, "ToolkitVersion") == 0) {
                    const char *val = "9.4";
                    DBusMessageIter variant;
                    dbus_message_iter_open_container(&reply_iter,
                        DBUS_TYPE_VARIANT, DBUS_TYPE_STRING_AS_STRING, &variant);
                    dbus_message_iter_append_basic(&variant, DBUS_TYPE_STRING, &val);
                    dbus_message_iter_close_container(&reply_iter, &variant);
                    handled = true;
                }
                else if (strcmp(prop_name, "AtspiVersion") == 0) {
                    const char *val = "2.1";
                    DBusMessageIter variant;
                    dbus_message_iter_open_container(&reply_iter,
                        DBUS_TYPE_VARIANT, DBUS_TYPE_STRING_AS_STRING, &variant);
                    dbus_message_iter_append_basic(&variant, DBUS_TYPE_STRING, &val);
                    dbus_message_iter_close_container(&reply_iter, &variant);
                    handled = true;
                }
                else if (strcmp(prop_name, "Id") == 0) {
                    uint32_t id = _app_id;
                    DBusMessageIter variant;
                    dbus_message_iter_open_container(&reply_iter,
                        DBUS_TYPE_VARIANT, DBUS_TYPE_UINT32_AS_STRING, &variant);
                    dbus_message_iter_append_basic(&variant, DBUS_TYPE_UINT32, &id);
                    dbus_message_iter_close_container(&reply_iter, &variant);
                    handled = true;
                }
            }

            if (!handled) {
                dbus_message_unref(*reply);
                *reply = nullptr;
                return false;
            }
            return true;
        }
        else if (strcmp(method, "Set") == 0) {
            // The registry sets Application:Id during registration
            DBusMessageIter iter;
            dbus_message_iter_init(args, &iter);

            const char *iface_name = nullptr, *prop_name = nullptr;
            if (!iterGetString(&iter, &iface_name)) return false;
            if (!dbus_message_iter_next(&iter)) return false;
            if (!iterGetString(&iter, &prop_name)) return false;

            if (strcmp(iface_name, ATSPI_INTERFACE_APPLICATION) == 0 &&
                strcmp(prop_name, "Id") == 0) {
                // Read the value
                if (!dbus_message_iter_next(&iter)) return false;
                DBusMessageIter variant;
                dbus_message_iter_recurse(&iter, &variant);
                uint32_t id = 0;
                if (dbus_message_iter_get_arg_type(&variant) == DBUS_TYPE_UINT32) {
                    dbus_message_iter_get_basic(&variant, &id);
                    _app_id = id;
                    printf("[atspi] Registry set Application.Id = %u\n", id);
                }
            }

            *reply = dbus_message_new_method_return(args);
            return true;
        }
        else if (strcmp(method, "GetAll") == 0) {
            // Minimal GetAll impl — return toolkit info
            *reply = dbus_message_new_method_return(args);
            DBusMessageIter reply_iter;
            dbus_message_iter_init_append(*reply, &reply_iter);

            DBusMessageIter dict_iter;
            dbus_message_iter_open_container(&reply_iter, DBUS_TYPE_ARRAY,
                DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
                DBUS_TYPE_STRING_AS_STRING
                DBUS_TYPE_VARIANT_AS_STRING
                DBUS_DICT_ENTRY_END_CHAR_AS_STRING, &dict_iter);

            // Put at least Name
            if (strcmp(iface_name, ATSPI_INTERFACE_ACCESSIBLE) == 0 && node) {
                std::string name = node->getName();
                DBusMessageIter entry, variant;
                dbus_message_iter_open_container(&dict_iter, DBUS_TYPE_DICT_ENTRY,
                                                  nullptr, &entry);
                const char *key = "Name";
                dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &key);
                dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT,
                    DBUS_TYPE_STRING_AS_STRING, &variant);
                const char *cstr = name.c_str();
                dbus_message_iter_append_basic(&variant, DBUS_TYPE_STRING, &cstr);
                dbus_message_iter_close_container(&entry, &variant);
                dbus_message_iter_close_container(&dict_iter, &entry);
            }

            dbus_message_iter_close_container(&reply_iter, &dict_iter);
            return true;
        }

        return false;
    }

    // Method calls on AT-SPI interfaces
    DBusMessageIter iter;
    dbus_message_iter_init(args, &iter);

    // --- org.a11y.atspi.Accessible members ---
    if (strcmp(interface, ATSPI_INTERFACE_ACCESSIBLE) == 0) {
        *reply = dbus_message_new_method_return(args);
        DBusMessageIter reply_iter;
        dbus_message_iter_init_append(*reply, &reply_iter);

        if (strcmp(method, "GetRole") == 0) {
            uint32_t role = node ? (uint32_t)node->getRole() : (uint32_t)AtspiRole::WINDOW;
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_UINT32, &role);
            return true;
        }
        else if (strcmp(method, "GetRoleName") == 0) {
            std::string name = node ? node->getRoleName() : "window";
            const char *cstr = name.c_str();
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_STRING, &cstr);
            return true;
        }
        else if (strcmp(method, "GetChildren") == 0) {
            auto children = node ? node->getChildren()
                                 : std::vector<std::pair<std::string, std::string>>();
            AtspiDbus::appendArrayOfRefs(&reply_iter, children);
            return true;
        }
        else if (strcmp(method, "GetChildAtIndex") == 0) {
            int32_t index = 0;
            if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_INT32) {
                dbus_message_iter_get_basic(&iter, &index);
            }
            auto child = node ? node->getChildAtIndex(index)
                              : std::pair<std::string, std::string>{
                                    "", ATSPI_DBUS_PATH_NULL};
            AtspiDbus::appendRef(&reply_iter, _app_bus_name.c_str(),
                                  child.second.c_str());
            return true;
        }
        else if (strcmp(method, "GetIndexInParent") == 0) {
            int32_t idx = node ? node->getIndexInParent() : -1;
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_INT32, &idx);
            return true;
        }
        else if (strcmp(method, "GetState") == 0) {
            uint64_t state = node ? node->getState() : (uint64_t)0;
            // AT-SPI expects an array of uint32 (legacy), not uint64
            uint32_t state_low = (uint32_t)(state & 0xFFFFFFFF);
            uint32_t state_high = (uint32_t)(state >> 32);
            uint32_t states[2] = {state_low, state_high};
            AtspiDbus::appendArrayOfInts(&reply_iter, states, 2);
            return true;
        }
        else if (strcmp(method, "GetApplication") == 0) {
            AtspiDbus::appendRef(&reply_iter, _app_bus_name.c_str(),
                                  ATSPI_DBUS_PATH_ROOT);
            return true;
        }
        else if (strcmp(method, "GetInterfaces") == 0) {
            auto ifaces = node ? node->getInterfaces()
                               : std::vector<std::string>{ATSPI_INTERFACE_ACCESSIBLE,
                                                           ATSPI_INTERFACE_APPLICATION,
                                                           ATSPI_INTERFACE_COMPONENT};
            DBusMessageIter sub;
            dbus_message_iter_open_container(&reply_iter, DBUS_TYPE_ARRAY,
                                              DBUS_TYPE_STRING_AS_STRING, &sub);
            for (auto &iface : ifaces) {
                const char *cstr = iface.c_str();
                dbus_message_iter_append_basic(&sub, DBUS_TYPE_STRING, &cstr);
            }
            dbus_message_iter_close_container(&reply_iter, &sub);
            return true;
        }
        else if (strcmp(method, "GetAttributes") == 0) {
            auto attrs = node ? node->getAttributes()
                              : std::vector<std::pair<std::string, std::string>>();
            AtspiDbus::appendStringDict(&reply_iter, attrs);
            return true;
        }
        else if (strcmp(method, "GetRelationSet") == 0) {
            // Return empty array of (u, a(so))
            DBusMessageIter sub;
            dbus_message_iter_open_container(&reply_iter, DBUS_TYPE_ARRAY,
                                              "(ua(so))", &sub);
            dbus_message_iter_close_container(&reply_iter, &sub);
            return true;
        }
        else if (strcmp(method, "GetLocale") == 0) {
            const char *locale = "C";
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_STRING, &locale);
            return true;
        }

        // Not handled — free reply and return false
        dbus_message_unref(*reply);
        *reply = nullptr;
        return false;
    }

    // --- org.a11y.atspi.Component ---
    if (strcmp(interface, ATSPI_INTERFACE_COMPONENT) == 0) {
        *reply = dbus_message_new_method_return(args);
        DBusMessageIter reply_iter;
        dbus_message_iter_init_append(*reply, &reply_iter);

        if (strcmp(method, "GetExtents") == 0) {
            int x = 0, y = 0, w = 0, h = 0;
            uint32_t coord_type = 0;
            if (node) node->getExtents(x, y, w, h, coord_type);
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_INT32, &x);
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_INT32, &y);
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_INT32, &w);
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_INT32, &h);
            return true;
        }
        else if (strcmp(method, "GetPosition") == 0) {
            int x = 0, y = 0;
            uint32_t coord_type = 0;
            if (node) node->getPosition(x, y, coord_type);
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_INT32, &x);
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_INT32, &y);
            return true;
        }
        else if (strcmp(method, "GetSize") == 0) {
            int w = 0, h = 0;
            if (node) node->getSize(w, h);
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_INT32, &w);
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_INT32, &h);
            return true;
        }
        else if (strcmp(method, "GetLayer") == 0) {
            uint32_t layer = node ? node->getLayer() : LAYER_WIDGET;
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_UINT32, &layer);
            return true;
        }
        else if (strcmp(method, "GrabFocus") == 0) {
            uint32_t ok = node && node->grabFocus() ? 1 : 0;
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_UINT32, &ok);
            return true;
        }
        else if (strcmp(method, "Contains") == 0) {
            int x = 0, y = 0;
            uint32_t coord_type = 0;
            if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_INT32) {
                dbus_message_iter_get_basic(&iter, &x);
                dbus_message_iter_next(&iter);
            }
            if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_INT32) {
                dbus_message_iter_get_basic(&iter, &y);
                dbus_message_iter_next(&iter);
            }
            if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_UINT32) {
                dbus_message_iter_get_basic(&iter, &coord_type);
            }
            uint32_t ok = node && node->contains(x, y, coord_type) ? 1 : 0;
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_UINT32, &ok);
            return true;
        }
        else if (strcmp(method, "GetAccessibleAtPoint") == 0) {
            int x = 0, y = 0;
            uint32_t coord_type = 0;
            if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_INT32) {
                dbus_message_iter_get_basic(&iter, &x);
                dbus_message_iter_next(&iter);
            }
            if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_INT32) {
                dbus_message_iter_get_basic(&iter, &y);
                dbus_message_iter_next(&iter);
            }
            if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_UINT32) {
                dbus_message_iter_get_basic(&iter, &coord_type);
            }
            auto ref = node ? node->getAccessibleAtPoint(x, y, coord_type)
                            : std::pair<std::string, std::string>{};
            AtspiDbus::appendRef(&reply_iter,
                                  ref.first.empty() ? _app_bus_name.c_str() : ref.first.c_str(),
                                  ref.second.empty() ? ATSPI_DBUS_PATH_NULL : ref.second.c_str());
            return true;
        }

        dbus_message_unref(*reply);
        *reply = nullptr;
        return false;
    }

    // --- org.a11y.atspi.Action ---
    if (strcmp(interface, ATSPI_INTERFACE_ACTION) == 0) {
        *reply = dbus_message_new_method_return(args);
        DBusMessageIter reply_iter;
        dbus_message_iter_init_append(*reply, &reply_iter);

        if (strcmp(method, "GetNActions") == 0) {
            int32_t n = node ? node->getNActions() : 0;
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_INT32, &n);
            return true;
        }
        else if (strcmp(method, "GetName") == 0) {
            int32_t index = 0;
            if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_INT32) {
                dbus_message_iter_get_basic(&iter, &index);
            }
            std::string name = node ? node->getActionName(index) : "";
            const char *cstr = name.c_str();
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_STRING, &cstr);
            return true;
        }
        else if (strcmp(method, "GetDescription") == 0) {
            int32_t index = 0;
            if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_INT32) {
                dbus_message_iter_get_basic(&iter, &index);
            }
            std::string desc = node ? node->getActionDescription(index) : "";
            const char *cstr = desc.c_str();
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_STRING, &cstr);
            return true;
        }
        else if (strcmp(method, "DoAction") == 0) {
            int32_t index = 0;
            if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_INT32) {
                dbus_message_iter_get_basic(&iter, &index);
            }
            uint32_t ok = node && node->doAction(index) ? 1 : 0;
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_UINT32, &ok);
            return true;
        }

        dbus_message_unref(*reply);
        *reply = nullptr;
        return false;
    }

    // --- org.a11y.atspi.Value ---
    if (strcmp(interface, ATSPI_INTERFACE_VALUE) == 0) {
        *reply = dbus_message_new_method_return(args);
        DBusMessageIter reply_iter;
        dbus_message_iter_init_append(*reply, &reply_iter);

        if (strcmp(method, "GetCurrentValue") == 0 ||
            strcmp(method, "getCurrentValue") == 0) {
            double val = node ? node->getCurrentValue() : 0.0;
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_DOUBLE, &val);
            return true;
        }
        else if (strcmp(method, "GetMinimumValue") == 0) {
            double val = node ? node->getMinimumValue() : 0.0;
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_DOUBLE, &val);
            return true;
        }
        else if (strcmp(method, "GetMaximumValue") == 0) {
            double val = node ? node->getMaximumValue() : 100.0;
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_DOUBLE, &val);
            return true;
        }
        else if (strcmp(method, "GetMinimumIncrement") == 0) {
            double val = node ? node->getMinimumIncrement() : 1.0;
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_DOUBLE, &val);
            return true;
        }
        else if (strcmp(method, "SetCurrentValue") == 0) {
            double val = 0.0;
            if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_DOUBLE) {
                dbus_message_iter_get_basic(&iter, &val);
            }
            bool ok = node && node->setCurrentValue(val);
            uint32_t uok = ok ? 1 : 0;
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_UINT32, &uok);
            return true;
        }

        dbus_message_unref(*reply);
        *reply = nullptr;
        return false;
    }

    // --- org.a11y.atspi.Application ---
    if (strcmp(interface, ATSPI_INTERFACE_APPLICATION) == 0) {
        *reply = dbus_message_new_method_return(args);
        DBusMessageIter reply_iter;
        dbus_message_iter_init_append(*reply, &reply_iter);

        if (strcmp(method, "GetLocale") == 0) {
            const char *locale = "C";
            dbus_message_iter_append_basic(&reply_iter, DBUS_TYPE_STRING, &locale);
            return true;
        }

        dbus_message_unref(*reply);
        *reply = nullptr;
        return false;
    }

    // --- org.a11y.atspi.Event.Object ---
    // (events are emitted as signals, not handled as method calls)

    return false;
}
