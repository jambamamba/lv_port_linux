#pragma once

#include <cstdint>

// AT-SPI2 D-Bus bus names, object paths, and interfaces
// Based on at-spi2-core v2.60 (https://gitlab.gnome.org/GNOME/at-spi2-core)

// --- Bus names & object paths ---
constexpr auto ATSPI_DBUS_NAME_REGISTRY   = "org.a11y.atspi.Registry";
constexpr auto ATSPI_DBUS_PATH_REGISTRY   = "/org/a11y/atspi/registry";
constexpr auto ATSPI_DBUS_PATH_ROOT       = "/org/a11y/atspi/accessible/root";
constexpr auto ATSPI_DBUS_PATH_NULL       = "/org/a11y/atspi/null";
constexpr auto ATSPI_DBUS_PATH_DEC       = "/org/a11y/atspi/accessible/dec";

// --- D-Bus interfaces ---
constexpr auto ATSPI_INTERFACE_ACCESSIBLE   = "org.a11y.atspi.Accessible";
constexpr auto ATSPI_INTERFACE_APPLICATION  = "org.a11y.atspi.Application";
constexpr auto ATSPI_INTERFACE_ACTION       = "org.a11y.atspi.Action";
constexpr auto ATSPI_INTERFACE_COMPONENT    = "org.a11y.atspi.Component";
constexpr auto ATSPI_INTERFACE_TEXT         = "org.a11y.atspi.Text";
constexpr auto ATSPI_INTERFACE_EDITABLE_TEXT = "org.a11y.atspi.EditableText";
constexpr auto ATSPI_INTERFACE_VALUE        = "org.a11y.atspi.Value";
constexpr auto ATSPI_INTERFACE_SELECTION    = "org.a11y.atspi.Selection";
constexpr auto ATSPI_INTERFACE_IMAGE        = "org.a11y.atspi.Image";
constexpr auto ATSPI_INTERFACE_HYPERTEXT    = "org.a11y.atspi.Hypertext";
constexpr auto ATSPI_INTERFACE_HYPERLINK    = "org.a11y.atspi.Hyperlink";
constexpr auto ATSPI_INTERFACE_SOCKET       = "org.a11y.atspi.Socket";
constexpr auto ATSPI_INTERFACE_REGISTRY     = "org.a11y.atspi.Registry";
constexpr auto ATSPI_INTERFACE_CACHE        = "org.a11y.atspi.Cache";
constexpr auto ATSPI_INTERFACE_COLLECTION   = "org.a11y.atspi.Collection";
constexpr auto ATSPI_INTERFACE_DOCUMENT     = "org.a11y.atspi.Document";

// --- Event interfaces ---
constexpr auto ATSPI_INTERFACE_EVENT_OBJECT   = "org.a11y.atspi.Event.Object";
constexpr auto ATSPI_INTERFACE_EVENT_WINDOW   = "org.a11y.atspi.Event.Window";
constexpr auto ATSPI_INTERFACE_EVENT_FOCUS    = "org.a11y.atspi.Event.Focus";
constexpr auto ATSPI_INTERFACE_EVENT_KEYBOARD = "org.a11y.atspi.Event.Keyboard";
constexpr auto ATSPI_INTERFACE_EVENT_MOUSE    = "org.a11y.atspi.Event.Mouse";

// --- AT-SPI Role enum (subset of ~130 roles) ---
enum class AtspiRole : uint32_t {
    INVALID              = 0,
    LABEL                = 34,
    PUSH_BUTTON          = 41,
    TOGGLE_BUTTON        = 42,
    CHECK_BOX            = 43,
    RADIO_BUTTON         = 44,
    COMBO_BOX            = 46,
    TEXT                 = 62,
    SLIDER               = 67,
    SCROLL_BAR           = 68,
    PROGRESS_BAR         = 69,
    CANVAS               = 73,
    IMAGE                = 76,
    LIST                 = 79,
    LIST_ITEM            = 80,
    MENU                 = 83,
    MENU_ITEM            = 84,
    PAGE_TAB             = 87,
    PAGE_TAB_LIST        = 88,
    PANEL                = 89,
    SEPARATOR            = 101,
    TABLE                = 109,
    TABLE_CELL           = 110,
    TOOL_TIP             = 112,
    TREE                 = 113,
    TREE_TABLE           = 114,
    WINDOW               = 117,
    FRAME                = 118,
    STATUS_BAR           = 121,
    COLOR_CHOOSER        = 128,
    TOGGLE_SWITCH        = 130,
    DIALOG               = 132,
};

// --- AT-SPI State enum (flags used as bitset) ---
enum AtspiState : uint64_t {
    STATE_INVALID            = 0,
    STATE_ACTIVE             = UINT64_C(1) << 0,
    STATE_ARMED              = UINT64_C(1) << 1,
    STATE_BUSY               = UINT64_C(1) << 2,
    STATE_CHECKED            = UINT64_C(1) << 3,
    STATE_COLLAPSED          = UINT64_C(1) << 5,
    STATE_DEFUNCT            = UINT64_C(1) << 7,
    STATE_EDITABLE           = UINT64_C(1) << 8,
    STATE_ENABLED            = UINT64_C(1) << 9,
    STATE_EXPANDABLE         = UINT64_C(1) << 10,
    STATE_EXPANDED           = UINT64_C(1) << 11,
    STATE_FOCUSABLE          = UINT64_C(1) << 12,
    STATE_FOCUSED            = UINT64_C(1) << 13,
    STATE_HAS_TOOLTIP        = UINT64_C(1) << 15,
    STATE_HORIZONTAL         = UINT64_C(1) << 16,
    STATE_ICONIFIED          = UINT64_C(1) << 18,
    STATE_MODAL              = UINT64_C(1) << 19,
    STATE_MULTI_LINE         = UINT64_C(1) << 20,
    STATE_MULTI_SELECTABLE   = UINT64_C(1) << 21,
    STATE_OPAQUE             = UINT64_C(1) << 22,
    STATE_PRESSED            = UINT64_C(1) << 23,
    STATE_RESIZABLE          = UINT64_C(1) << 24,
    STATE_SELECTABLE         = UINT64_C(1) << 26,
    STATE_SELECTED           = UINT64_C(1) << 27,
    STATE_SENSITIVE          = UINT64_C(1) << 28,
    STATE_SHOWING            = UINT64_C(1) << 29,
    STATE_TRANSIENT          = UINT64_C(1) << 31,
    STATE_VERTICAL           = UINT64_C(1) << 33,
    STATE_VISIBLE            = UINT64_C(1) << 34,
    STATE_MANAGES_DESCENDANTS = UINT64_C(1) << 35,
    STATE_INDETERMINATE      = UINT64_C(1) << 36,
    STATE_REQUIRED           = UINT64_C(1) << 38,
    STATE_TRUNCATED          = UINT64_C(1) << 39,
    STATE_ANIMATED           = UINT64_C(1) << 40,
    STATE_INVALID_ENTRY      = UINT64_C(1) << 41,
    STATE_SUPPORTS_AUTOCOMPLETION = UINT64_C(1) << 44,
    STATE_SELECTABLE_TEXT    = UINT64_C(1) << 45,
    STATE_IS_DEFAULT         = UINT64_C(1) << 46,
    STATE_VISITED            = UINT64_C(1) << 47,
    STATE_CHECKABLE          = UINT64_C(1) << 48,
    STATE_HAS_POPUP          = UINT64_C(1) << 49,
    STATE_READ_ONLY          = UINT64_C(1) << 51,
};

// --- Coordinate types ---
enum AtspiCoordType : uint32_t {
    COORD_SCREEN  = 0,
    COORD_WINDOW  = 1,
    COORD_PARENT  = 2,
};

// --- Layer enum (Component::GetLayer) ---
enum AtspiLayer : uint32_t {
    LAYER_BACKGROUND = 0,
    LAYER_CANVAS     = 1,
    LAYER_WIDGET     = 2,
    LAYER_MDI        = 3,
    LAYER_WINDOW     = 4,
    LAYER_OVERLAY    = 5,
};

// --- Event signal body: (source s, detail1 i, detail2 i, data v, properties a{sv}) ---
// All AT-SPI events use this 5-tuple signature.
constexpr auto EVENT_SIGNATURE = "siiv";
constexpr auto EVENT_SIGNATURE_FULL = "siiva{sv}";

// --- Relation types ---
enum AtspiRelationType : uint32_t {
    RELATION_NULL                = 0,
    RELATION_LABEL_FOR           = 1,
    RELATION_LABELLED_BY         = 2,
    RELATION_CONTROLLER_FOR      = 3,
    RELATION_CONTROLLED_BY       = 4,
    RELATION_MEMBER_OF           = 6,
    RELATION_TOOLTIP_FOR         = 7,
    RELATION_NODE_CHILD_OF       = 8,
    RELATION_PARENT_WINDOW_OF    = 11,
    RELATION_SUBWINDOW_OF        = 12,
    RELATION_EMBEDS              = 13,
    RELATION_EMBEDDED_BY         = 14,
    RELATION_DESCRIPTION_FOR     = 16,
    RELATION_DESCRIBED_BY        = 17,
    RELATION_TERM                = 19,
    RELATION_DEFINITION          = 20,
    RELATION_FLOWS_TO            = 21,
    RELATION_FLOWS_FROM          = 22,
};
