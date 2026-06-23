#pragma once

#include <string>
#include <vector>
#include <dbus/dbus.h>

// Thin wrapper around libdbus-1 for AT-SPI2 protocol needs.
// Handles: connection to session bus, exporting objects,
// method dispatch, signal emission, introspection.

struct AtspiDbus {
    // Initialize connection to the session D-Bus bus.
    // Returns true if the session bus is reachable.
    static bool connect();

    // Disconnect and free all resources.
    static void disconnect();

    static DBusConnection *conn();

    // Register an object path with method handlers.
    // `handler` receives (path, interface, method, args) and returns a reply.
    using MethodHandler = bool (*)(const char *path,
                                   const char *interface,
                                   const char *method,
                                   DBusMessage *args,
                                   DBusMessage **reply);
    static void registerObject(const char *path, MethodHandler handler);

    // Send a method call and wait for a reply.
    static DBusMessage *call(const char *bus_name,
                             const char *path,
                             const char *interface,
                             const char *method,
                             int first_arg_type,
                             ...);

    // Emit a D-Bus signal (fire-and-forget).
    static bool emitSignal(const char *path,
                           const char *interface,
                           const char *signal_name,
                           DBusMessage *msg);

    // Build a standard AT-SPI reference (so) tuple string: "(:1.123,/path)"
    static std::string makeRef(const char *bus_name, const char *path);

    // Build an AT-SPI (so) variant suitable for appending to a message.
    static bool appendRef(DBusMessageIter *iter,
                          const char *bus_name,
                          const char *path);

    static bool appendRef(DBusMessage *msg,
                          const char *bus_name,
                          const char *path);

    // Helpers for appending AT-SPI types
    static bool appendArrayOfInts(DBusMessageIter *iter, const uint32_t *vals, int count);
    static bool appendArrayOfRefs(DBusMessageIter *iter,
                                  const std::vector<std::pair<std::string, std::string>> &refs);
    static bool appendDictEntry(DBusMessageIter *dict_iter,
                                const char *key, const char *value);
    static bool appendStringDict(DBusMessageIter *iter,
                                 const std::vector<std::pair<std::string, std::string>> &entries);

private:
    static DBusConnection *_conn;
    static DBusObjectPathVTable _vtable;

    static DBusHandlerResult dispatch(DBusConnection *c,
                                      DBusMessage *msg,
                                      void *user_data);
};
