#include "accessible_node.h"
#include "atspi_dbus.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

// --- LVGL class → AT-SPI role mapping ---

AtspiRole lvglClassToAtspiRole(const char *name) {
    if (!name) return AtspiRole::PANEL;

    // Lele widget mappings
    if (strstr(name, "lelebutton") || strstr(name, "lv_button")) return AtspiRole::PUSH_BUTTON;
    if (strstr(name, "lelelabel")  || strstr(name, "lv_label"))  return AtspiRole::LABEL;
    if (strstr(name, "leleimage")  || strstr(name, "lv_image") || strstr(name, "lv_img")) return AtspiRole::IMAGE;
    if (strstr(name, "leleslider") || strstr(name, "lelebar")   || strstr(name, "lv_slider") || strstr(name, "lv_bar")) return AtspiRole::SLIDER;
    if (strstr(name, "leletextbox")|| strstr(name, "lv_textarea")) return AtspiRole::TEXT;
    if (strstr(name, "leletabview")|| strstr(name, "lv_tabview")) return AtspiRole::PAGE_TAB_LIST;
    if (strstr(name, "lelestackview")) return AtspiRole::PANEL;
    if (strstr(name, "leleview")  || strstr(name, "lv_obj") || strstr(name, "lv_view")) return AtspiRole::PANEL;
    if (strstr(name, "lelemessagebox")) return AtspiRole::DIALOG; // 68
    if (strstr(name, "lelecolorwheel")) return AtspiRole::COLOR_CHOOSER; // 64
    if (strstr(name, "lelerollerview")) return AtspiRole::LIST;
    if (strstr(name, "lv_checkbox")) return AtspiRole::CHECK_BOX;
    if (strstr(name, "lv_switch"))  return AtspiRole::TOGGLE_SWITCH;
    if (strstr(name, "lv_dropdown")) return AtspiRole::COMBO_BOX;
    if (strstr(name, "lv_roller"))  return AtspiRole::LIST;
    if (strstr(name, "lv_spinbox")) return AtspiRole::SPIN_BUTTON; // 66
    if (strstr(name, "lv_table"))   return AtspiRole::TABLE;
    if (strstr(name, "lv_win") || strstr(name, "lv_window")) return AtspiRole::WINDOW;

    return AtspiRole::PANEL;
}

// --- AccessibleNode implementation ---

std::string AccessibleNode::getName() const {
    // Use LVGL object ID if set, otherwise fall back to class name
    const char *id = lv_obj_get_id(lv_obj);
    if (id && strlen(id) > 0) return id;

    // Try the Lele widget's ID (set from JSON "id" field)
    // This is stored as user_data on the LVGL object by our framework
    // For now, use a heuristic: the `lv_obj_get_user_data` might hold it
    return widgetClassName(lv_obj);
}

std::string AccessibleNode::getDescription() const {
    // Could be extended with a custom attribute or a11y description
    // stored as LVGL object user data. For now, empty.
    return "";
}

std::pair<std::string, std::string> AccessibleNode::getParent() const {
    if (parent) return {AtspiDbus::makeRef(nullptr, parent->path.c_str()), parent->path};
    return {"", ATSPI_DBUS_PATH_NULL};
}

int AccessibleNode::getChildCount() const {
    return (int)children.size();
}

int AccessibleNode::getIndexInParent() const {
    if (!parent) return 0;
    for (size_t i = 0; i < parent->children.size(); i++) {
        if (parent->children[i] == this) return (int)i;
    }
    return 0;
}

std::pair<std::string, std::string> AccessibleNode::getChildAtIndex(int index) const {
    if (index < 0 || index >= (int)children.size()) {
        return {"", ATSPI_DBUS_PATH_NULL};
    }
    auto *child = children[index];
    return {AtspiDbus::makeRef(nullptr, child->path.c_str()), child->path};
}

std::vector<std::pair<std::string, std::string>> AccessibleNode::getChildren() const {
    std::vector<std::pair<std::string, std::string>> result;
    for (auto *child : children) {
        result.push_back({child->path, child->path});
    }
    return result;
}

uint32_t AccessibleNode::getRole() const {
    return (uint32_t)role;
}

std::string AccessibleNode::getRoleName() const {
    // Very basic; a full implementation would have a reverse lookup table
    switch (role) {
        case AtspiRole::PUSH_BUTTON:  return "push button";
        case AtspiRole::LABEL:         return "label";
        case AtspiRole::SLIDER:        return "slider";
        case AtspiRole::IMAGE:         return "image";
        case AtspiRole::TEXT:          return "text";
        case AtspiRole::PANEL:         return "panel";
        case AtspiRole::WINDOW:        return "window";
        case AtspiRole::CHECK_BOX:     return "check box";
        case AtspiRole::TOGGLE_SWITCH: return "toggle switch";
        case AtspiRole::COMBO_BOX:     return "combo box";
        case AtspiRole::LIST:          return "list";
        case AtspiRole::TABLE:         return "table";
        case AtspiRole::PAGE_TAB_LIST: return "page tab list";
        default:                       return "unknown";
    }
}

uint64_t AccessibleNode::getState() const {
    return computeState();
}

std::vector<std::pair<std::string, std::string>> AccessibleNode::getAttributes() const {
    return {};
}

std::vector<std::string> AccessibleNode::getInterfaces() const {
    std::vector<std::string> ifaces;
    ifaces.push_back(ATSPI_INTERFACE_ACCESSIBLE);
    ifaces.push_back(ATSPI_INTERFACE_COMPONENT);

    if (supportsAction())  ifaces.push_back(ATSPI_INTERFACE_ACTION);
    if (supportsValue())   ifaces.push_back(ATSPI_INTERFACE_VALUE);
    if (supportsText())    ifaces.push_back(ATSPI_INTERFACE_TEXT);

    return ifaces;
}

uint64_t AccessibleNode::computeState() const {
    uint64_t state = 0;

    if (lv_obj_has_flag(lv_obj, LV_OBJ_FLAG_HIDDEN) == false)
        state |= STATE_VISIBLE;
    if (lv_obj_has_flag(lv_obj, LV_OBJ_FLAG_CLICKABLE))
        state |= STATE_SENSITIVE | STATE_ENABLED;
    if (lv_obj_has_flag(lv_obj, LV_OBJ_FLAG_CHECKABLE))
        state |= STATE_CHECKABLE;
    if (lv_obj_has_flag(lv_obj, LV_OBJ_FLAG_CLICK_FOCUSABLE))
        state |= STATE_FOCUSABLE;

    // Focused?
    if (lv_obj_has_state(lv_obj, LV_STATE_FOCUSED))
        state |= STATE_FOCUSED;

    // Showing on screen?
    if (lv_obj_is_scrolling(lv_obj) == false) {
        state |= STATE_SHOWING;
    }

    return state;
}

void AccessibleNode::rebuildStateCache() {
    cached_state = computeState();
}

bool AccessibleNode::supportsAction() const {
    switch (role) {
        case AtspiRole::PUSH_BUTTON:
        case AtspiRole::TOGGLE_BUTTON:
        case AtspiRole::CHECK_BOX:
        case AtspiRole::COMBO_BOX:
            return true;
        default:
            return false;
    }
}

int AccessibleNode::getNActions() const {
    return supportsAction() ? 1 : 0;
}

std::string AccessibleNode::getActionName(int index) const {
    if (index == 0) return "click";
    return "";
}

std::string AccessibleNode::getActionDescription(int index) const {
    if (index == 0) return "press the button";
    return "";
}

std::string AccessibleNode::getActionKeyBinding(int index) const {
    return "";
}

bool AccessibleNode::doAction(int index) {
    if (index == 0 && lv_obj_has_flag(lv_obj, LV_OBJ_FLAG_CLICKABLE)) {
        // Simulate a click on the LVGL widget
        lv_obj_send_event(lv_obj, LV_EVENT_CLICKED, nullptr);
        return true;
    }
    return false;
}

// --- Component interface ---

bool AccessibleNode::contains(int x, int y, uint32_t coord_type) const {
    // TODO: handle coord_type properly (screen vs. window)
    lv_area_t coords;
    lv_obj_get_coords(lv_obj, &coords);
    return x >= coords.x1 && x <= coords.x2 && y >= coords.y1 && y <= coords.y2;
}

std::pair<std::string, std::string>
AccessibleNode::getAccessibleAtPoint(int x, int y, uint32_t coord_type) const {
    // Walk children in reverse (top-most first) to find the deepest match
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        auto *child = *it;
        if (child->contains(x, y, coord_type)) {
            return child->getAccessibleAtPoint(x, y, coord_type);
        }
    }
    // This node is the deepest match
    return {AtspiDbus::makeRef(nullptr, path.c_str()), path};
}

void AccessibleNode::getExtents(int &x, int &y, int &w, int &h,
                                 uint32_t coord_type) const {
    (void)coord_type;
    lv_area_t coords;
    lv_obj_get_coords(lv_obj, &coords);
    x = coords.x1;
    y = coords.y1;
    w = coords.x2 - coords.x1 + 1;
    h = coords.y2 - coords.y1 + 1;
}

void AccessibleNode::getPosition(int &x, int &y, uint32_t coord_type) const {
    (void)coord_type;
    lv_area_t coords;
    lv_obj_get_coords(lv_obj, &coords);
    x = coords.x1;
    y = coords.y1;
}

void AccessibleNode::getSize(int &w, int &h) const {
    lv_area_t coords;
    lv_obj_get_coords(lv_obj, &coords);
    w = coords.x2 - coords.x1 + 1;
    h = coords.y2 - coords.y1 + 1;
}

uint32_t AccessibleNode::getLayer() const {
    // Root objects are windows; everything else is a widget
    if (isRoot()) return LAYER_WINDOW;
    return LAYER_WIDGET;
}

bool AccessibleNode::grabFocus() {
    lv_obj_t *screen = lv_obj_get_screen(lv_obj);
    lv_group_t *group = lv_group_get_default();
    if (!group) return false;
    lv_group_set_editing(group, lv_obj == screen);
    lv_obj_add_state(lv_obj, LV_STATE_FOCUSED);
    return true;
}

// --- Value interface ---

bool AccessibleNode::supportsValue() const {
    return role == AtspiRole::SLIDER;
}

double AccessibleNode::getMinimumValue() const {
    // TODO: query slider min from LVGL
    return 0.0;
}

double AccessibleNode::getMaximumValue() const {
    // TODO: query slider max from LVGL
    return 100.0;
}

double AccessibleNode::getMinimumIncrement() const {
    return 1.0;
}

double AccessibleNode::getCurrentValue() const {
    // TODO: query slider value from LVGL
    return 0.0;
}

bool AccessibleNode::setCurrentValue(double val) {
    // TODO: set slider value in LVGL
    (void)val;
    return false;
}

// --- Text interface ---

bool AccessibleNode::supportsText() const {
    return role == AtspiRole::LABEL || role == AtspiRole::TEXT;
}

int AccessibleNode::getCharacterCount() const {
    const char *text = lv_label_get_text(lv_obj);
    if (!text) return 0;
    return (int)strlen(text);
}

std::string AccessibleNode::getText(int start_offset, int end_offset) const {
    const char *text = lv_label_get_text(lv_obj);
    if (!text) return "";

    int len = (int)strlen(text);
    if (start_offset < 0) start_offset = 0;
    if (end_offset > len) end_offset = len;
    if (start_offset >= end_offset) return "";

    return std::string(text + start_offset, text + end_offset);
}

int AccessibleNode::getCaretOffset() const {
    return 0;
}

bool AccessibleNode::setCaretOffset(int offset) {
    (void)offset;
    return false;
}

bool AccessibleNode::isRoot() const {
    // Root node: no parent, or parent is the screen itself
    return parent == nullptr;
}
