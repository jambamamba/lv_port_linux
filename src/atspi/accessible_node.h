#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include <lvgl/lvgl.h>

#include "atspi_constants.h"

// Wraps a single LVGL object as an AT-SPI accessible node.
// Each node knows its LVGL object, its parent/children in the
// accessible tree, and which AT-SPI interfaces it implements.
struct AccessibleNode {
    lv_obj_t *lv_obj = nullptr;          // owning LVGL widget
    AccessibleNode *parent = nullptr;     // weak ref
    std::vector<AccessibleNode *> children;

    // Object path on D-Bus (e.g. "/org/a11y/atspi/accessible/dec/42")
    std::string path;

    // Cached state bitset so we can detect changes for events
    uint64_t cached_state = 0;

    // AT-SPI role for this widget (derived from widget class)
    AtspiRole role = AtspiRole::INVALID;

    explicit AccessibleNode(lv_obj_t *obj) : lv_obj(obj) {}

    // --- AT-SPI Accessible interface ---
    std::string getName() const;
    std::string getDescription() const;
    std::pair<std::string, std::string> getParent() const;  // (bus_name, path)
    int getChildCount() const;
    std::pair<std::string, std::string> getChildAtIndex(int index) const;
    std::vector<std::pair<std::string, std::string>> getChildren() const;
    int getIndexInParent() const;
    uint32_t getRole() const;
    std::string getRoleName() const;
    uint64_t getState() const;
    std::vector<std::pair<std::string, std::string>> getAttributes() const;
    std::vector<std::string> getInterfaces() const;

    // --- AT-SPI Component interface ---
    bool contains(int x, int y, uint32_t coord_type) const;
    std::pair<std::string, std::string> getAccessibleAtPoint(int x, int y,
                                                              uint32_t coord_type) const;
    void getExtents(int &x, int &y, int &w, int &h, uint32_t coord_type) const;
    void getPosition(int &x, int &y, uint32_t coord_type) const;
    void getSize(int &w, int &h) const;
    uint32_t getLayer() const;
    bool grabFocus();

    // --- AT-SPI Action interface ---
    int getNActions() const;
    std::string getActionName(int index) const;
    std::string getActionDescription(int index) const;
    std::string getActionKeyBinding(int index) const;
    bool doAction(int index);

    // --- AT-SPI Value interface (sliders, progress bars) ---
    bool supportsValue() const;
    double getMinimumValue() const;
    double getMaximumValue() const;
    double getMinimumIncrement() const;
    double getCurrentValue() const;
    bool setCurrentValue(double val);

    // --- AT-SPI Text interface ---
    bool supportsText() const;
    int getCharacterCount() const;
    std::string getText(int start_offset, int end_offset) const;
    int getCaretOffset() const;
    bool setCaretOffset(int offset);

    // Helpers
    bool isRoot() const;
    void rebuildStateCache();
    uint64_t computeState() const;
};

// LVGL widget class name → AtspiRole mapping
AtspiRole lvglClassToAtspiRole(const char *lvgl_class_name);

// Extract the widget class name from an LVGL object
inline const char *widgetClassName(lv_obj_t *obj) {
    const lv_obj_class_t *cls = lv_obj_get_class(obj);
    return cls ? cls->name : "unknown";
}
