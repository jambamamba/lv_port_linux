#include "atspi_dbus.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

DBusConnection *AtspiDbus::_conn = nullptr;
DBusObjectPathVTable AtspiDbus::_vtable = {};

// Registry of method handlers per object path
// (In production, use a proper hash map; this is simplified for the scaffold.)
// Key = path, value = handler
static std::vector<std::pair<std::string, AtspiDbus::MethodHandler>> s_handlers;

bool AtspiDbus::connect() {
    DBusError err;
    dbus_error_init(&err);

    _conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (!_conn) {
        fprintf(stderr, "[atspi] Failed to connect to session bus: %s\n",
                err.message);
        dbus_error_free(&err);
        return false;
    }

    dbus_connection_set_exit_on_disconnect(_conn, false);

    // Set up the vtable for dispatch
    memset(&_vtable, 0, sizeof(_vtable));
    _vtable.message_function = dispatch;

    printf("[atspi] Connected to D-Bus session bus (unique name: %s)\n",
           dbus_bus_get_unique_name(_conn));
    return true;
}

void AtspiDbus::disconnect() {
    if (_conn) {
        dbus_connection_unref(_conn);
        _conn = nullptr;
    }
    s_handlers.clear();
}

DBusConnection *AtspiDbus::conn() { return _conn; }

void AtspiDbus::registerObject(const char *path, MethodHandler handler) {
    // Register with libdbus
    if (!dbus_connection_register_object_path(_conn, path, &_vtable, nullptr)) {
        fprintf(stderr, "[atspi] Failed to register object path: %s\n", path);
        return;
    }
    s_handlers.push_back({path, handler});
}

DBusMessage *AtspiDbus::call(const char *bus_name,
                              const char *path,
                              const char *interface,
                              const char *method,
                              int first_arg_type,
                              ...) {
    DBusMessage *msg = dbus_message_new_method_call(bus_name, path, interface, method);
    if (!msg) return nullptr;

    if (first_arg_type != DBUS_TYPE_INVALID) {
        va_list ap;
        va_start(ap, first_arg_type);
        dbus_message_append_args_valist(msg, first_arg_type, ap);
        va_end(ap);
    }

    DBusError err;
    dbus_error_init(&err);

    DBusMessage *reply = dbus_connection_send_with_reply_and_block(_conn, msg, -1, &err);
    dbus_message_unref(msg);

    if (!reply) {
        fprintf(stderr, "[atspi] D-Bus call %s.%s failed: %s\n",
                interface, method, err.message);
        dbus_error_free(&err);
        return nullptr;
    }
    return reply;
}

bool AtspiDbus::emitSignal(const char *path,
                            const char *interface,
                            const char *signal_name,
                            DBusMessage *msg) {
    // If a msg is provided, use it directly; otherwise create one.
    DBusMessage *signal = msg;
    bool owned = false;
    if (!signal) {
        signal = dbus_message_new_signal(path, interface, signal_name);
        owned = true;
    }

    if (!signal) return false;

    // Set the destination to the registry so ATs receive it
    dbus_message_set_destination(signal, ATSPI_DBUS_NAME_REGISTRY);

    dbus_connection_send(_conn, signal, nullptr);
    dbus_connection_flush(_conn);

    if (owned) dbus_message_unref(signal);
    return true;
}

bool AtspiDbus::appendRef(DBusMessageIter *iter,
                           const char *bus_name,
                           const char *path) {
    // (so) tuple
    DBusMessageIter sub;
    dbus_message_iter_open_container(iter, DBUS_TYPE_STRUCT, nullptr, &sub);

    const char *name = bus_name ? bus_name : dbus_bus_get_unique_name(_conn);
    const char *obj_path = path ? path : ATSPI_DBUS_PATH_NULL;

    dbus_message_iter_append_basic(&sub, DBUS_TYPE_STRING, &name);
    dbus_message_iter_append_basic(&sub, DBUS_TYPE_OBJECT_PATH, &obj_path);

    dbus_message_iter_close_container(iter, &sub);
    return true;
}

bool AtspiDbus::appendRef(DBusMessage *msg,
                           const char *bus_name,
                           const char *path) {
    DBusMessageIter iter;
    dbus_message_iter_init_append(msg, &iter);
    return appendRef(&iter, bus_name, path);
}

std::string AtspiDbus::makeRef(const char *bus_name, const char *path) {
    return std::string("(") + (bus_name ? bus_name : dbus_bus_get_unique_name(_conn))
           + "," + (path ? path : ATSPI_DBUS_PATH_NULL) + ")";
}

bool AtspiDbus::appendArrayOfInts(DBusMessageIter *iter,
                                   const uint32_t *vals, int count) {
    DBusMessageIter sub;
    dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY,
                                      DBUS_TYPE_UINT32_AS_STRING, &sub);
    for (int i = 0; i < count; i++) {
        dbus_message_iter_append_basic(&sub, DBUS_TYPE_UINT32, &vals[i]);
    }
    dbus_message_iter_close_container(iter, &sub);
    return true;
}

bool AtspiDbus::appendArrayOfRefs(
    DBusMessageIter *iter,
    const std::vector<std::pair<std::string, std::string>> &refs) {
    DBusMessageIter sub;
    dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY,
                                      "(so)", &sub);
    for (auto &ref : refs) {
        appendRef(&sub, ref.first.c_str(), ref.second.c_str());
    }
    dbus_message_iter_close_container(iter, &sub);
    return true;
}

bool AtspiDbus::appendDictEntry(DBusMessageIter *dict_iter,
                                 const char *key, const char *value) {
    DBusMessageIter entry;
    dbus_message_iter_open_container(dict_iter, DBUS_TYPE_DICT_ENTRY,
                                      nullptr, &entry);
    dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &key);
    dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &value);
    dbus_message_iter_close_container(dict_iter, &entry);
    return true;
}

bool AtspiDbus::appendStringDict(
    DBusMessageIter *iter,
    const std::vector<std::pair<std::string, std::string>> &entries) {
    DBusMessageIter sub;
    dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY,
                                      DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
                                      DBUS_TYPE_STRING_AS_STRING
                                      DBUS_TYPE_STRING_AS_STRING
                                      DBUS_DICT_ENTRY_END_CHAR_AS_STRING, &sub);
    for (auto &e : entries) {
        appendDictEntry(&sub, e.first.c_str(), e.second.c_str());
    }
    dbus_message_iter_close_container(iter, &sub);
    return true;
}

DBusHandlerResult AtspiDbus::dispatch(DBusConnection *c,
                                       DBusMessage *msg,
                                       void *user_data) {
    (void)c;
    (void)user_data;

    int type = dbus_message_get_type(msg);
    if (type != DBUS_MESSAGE_TYPE_METHOD_CALL) {
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    const char *path = dbus_message_get_path(msg);
    const char *interface = dbus_message_get_interface(msg);
    const char *method = dbus_message_get_member(msg);

    if (!path || !interface || !method) {
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    // Handle org.freedesktop.DBus.Introspectable on all objects
    if (strcmp(interface, "org.freedesktop.DBus.Introspectable") == 0 &&
        strcmp(method, "Introspect") == 0) {
        // Return minimal introspection data (ATs rarely use this)
        const char *xml =
            "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\"\n"
            "\"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n"
            "<node>\n"
            "  <interface name=\"org.freedesktop.DBus.Introspectable\">\n"
            "    <method name=\"Introspect\">\n"
            "      <arg name=\"data\" direction=\"out\" type=\"s\"/>\n"
            "    </method>\n"
            "  </interface>\n"
            "  <interface name=\"org.a11y.atspi.Accessible\"/>\n"
            "  <interface name=\"org.a11y.atspi.Application\"/>\n"
            "  <interface name=\"org.a11y.atspi.Component\"/>\n"
            "  <interface name=\"org.a11y.atspi.Action\"/>\n"
            "  <interface name=\"org.a11y.atspi.Value\"/>\n"
            "</node>\n";

        DBusMessage *reply = dbus_message_new_method_return(msg);
        dbus_message_append_args(reply, DBUS_TYPE_STRING, &xml,
                                  DBUS_TYPE_INVALID);
        dbus_connection_send(_conn, reply, nullptr);
        dbus_message_unref(reply);
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    // Find a matching handler
    for (auto &entry : s_handlers) {
        // The called path *must start with* the registered prefix for
        // sub-objects to work; we check exact match for simplicity.
        if (path == entry.first || strcmp(path, entry.first.c_str()) == 0) {
            DBusMessage *reply = nullptr;
            if (entry.second(path, interface, method, msg, &reply)) {
                if (reply) {
                    dbus_connection_send(_conn, reply, nullptr);
                    dbus_message_unref(reply);
                }
                return DBUS_HANDLER_RESULT_HANDLED;
            }
        }
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}
