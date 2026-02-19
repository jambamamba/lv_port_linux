#pragma once


#include <debug_logger/debug_logger.h>
#include <json_utils/json_utils.h>
#include <lvgl/lvgl_private.h>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_set>
#include <variant>
#include <vector>
#include <Python.h>

class LeleObject;
struct PyLeleStyle;
typedef struct _object PyObject;
class LeleStyle {
public:
  enum BorderTypeE {
    None=-1,Solid,Dashed,Dotted
  };
  using StyleValue = std::variant<
    int,
    float,
    std::string,
    lv_layout_t,
    lv_flex_flow_t,
    lv_scrollbar_mode_t,
    LeleStyle::BorderTypeE
  >;
  // static LeleStyle fromJson(int parent_width = 0, int parent_height = 0, const cJSON *json = nullptr);
  // LeleStyle(int parent_width = 0, int parent_height = 0, const std::string &x = "", const std::string &y = "", const std::string &width = "", const std::string &height = "");
  LeleStyle(LeleObject *lele_obj = nullptr, const std::string &json_str = "");
  // LeleStyle(LeleObject *lele_obj = nullptr, const std::map<std::string, std::optional<StyleValue>> &style_attributes, lv_obj_t *parent = lv_screen_active());
  virtual bool fromJson(const std::string &json_str);
  virtual PyObject *createPyObject();
  virtual bool initPyObject(PyLeleStyle *py_obj);
  friend std::ostream& operator<<(std::ostream& os, const LeleStyle& p);
  const LeleObject *getLeleObject() const;
  static std::string trim(const std::string& str);
  void parseFlex(const std::string &value);
  static int parsePercentValue(const std::string &x, int parent_x);
  static std::tuple<LeleStyle::BorderTypeE,int,int> parseBorder(const std::string &border_type_width_color);
  std::string getClassName() const;
  std::optional<StyleValue> getValue(const std::string &key, const std::string &class_name = "") const;
  bool setValue(const std::string &key, const std::string &value);
  std::vector<std::string> getBackgroundAttributesAsOrderedInJson() const;
  const std::map<std::string, std::optional<StyleValue>> getStyle() const;
  std::string getId() const;
  void applyStyle();
  static std::map<std::string, std::vector<std::string>> _flex_possible_values;
  static std::map<std::string, std::map<std::string,int>> _flex_possible_ivalues;
  static std::unordered_set<std::string> _style_keys;

protected:
  std::tuple<int,int,int,int> parsePaddingOrMargin(const std::string &padding_str);
  std::tuple<std::string,std::string,std::string,std::string> parseTopRightBottomLeft(const std::string &value);

  LeleObject *_lele_obj = nullptr;
  std::string _class_name;
  std::string _id;
  std::map<std::string, std::optional<StyleValue>> _style = {};
  mutable std::vector<std::string> _background_attributes_as_ordered_in_json;
  int _parent_width = 0;
  int _parent_height = 0;
};

struct PyLeleStyle {
    PyObject ob_base;
    static PyTypeObject _obj_type;
    static PyMemberDef _members[];
    static PyMethodDef _methods[];
    static PyObject *toPyDict(
      LeleStyle *lele_style,
      const std::map<std::string, std::optional<LeleStyle::StyleValue>> &&style_name_value_map,
      const std::unordered_set<std::string> &&white_list = {});
    static void dealloc(PyObject* self);
    static int init(PyObject *self, PyObject *args, PyObject *kwds);
    static void dtor(PyObject *self_);
    static void onfree(PyObject *self_);
    // Type-specific fields go here
    std::vector<std::unique_ptr<LeleStyle>> _lele_styles;
    LeleStyle* _lele_style = nullptr;
    PyObject *_id = nullptr;
    PyObject *_class_name = nullptr;
    PyObject *_layout = nullptr;
    PyObject *_flow = nullptr;
    PyObject *_scrollbar = nullptr;
    PyObject *_border = nullptr;
    static PyObject *fromConfig(PyObject *, PyObject *);
    static PyObject *getClassName(PyObject *, PyObject *);
    static PyObject *getValue(PyObject *, PyObject *);
    static PyObject *setValue(PyObject *, PyObject *);
};

#define PY_LELESTYLE_MEMBERS() \
  {"id", Py_T_OBJECT_EX, offsetof(PyLeleStyle, _id), 0, "id"},\
  {"Layout", Py_T_OBJECT_EX, offsetof(PyLeleStyle, _layout), 0, "Layout"},\
  {"Flow", Py_T_OBJECT_EX, offsetof(PyLeleStyle, _flow), 0, "Flow"},\
  {"Scrollbar", Py_T_OBJECT_EX, offsetof(PyLeleStyle, _scrollbar), 0, "Scrollbar"},\
  {"Border", Py_T_OBJECT_EX, offsetof(PyLeleStyle, _border), 0, "Border"},\

#define PY_LELESTYLE_METHODS() \
  {"fromConfig", (PyCFunction)PyLeleStyle::fromConfig, METH_VARARGS, "Parent object. Json config file: The object is loaded from a configuration file with JSON description of the object"},\
  {"getClassName", (PyCFunction)PyLeleStyle::getClassName, METH_NOARGS, "Get the class name"},\
  {"getValue", (PyCFunction)PyLeleStyle::getValue, METH_VARARGS, "Get a dictionary with the style's attribute/value pairs"},\
  {"setValue", (PyCFunction)PyLeleStyle::setValue, METH_VARARGS, "Set the style from a dictionary containing the style's attribute/value pairs"},\

///////////////////////////////////////////////////
struct PyLeleStyleLayout {
    PyObject ob_base;
    static PyTypeObject _obj_type;
    static PyMemberDef _members[];
    static int init(PyObject *self, PyObject *args, PyObject *kwds);
    static void dealloc(PyObject* self);
    // Type-specific fields go here
    PyObject *_flex = nullptr;
    PyObject *_grid = nullptr;
    PyObject *_no = nullptr;
};

#define PY_LELESTYLE_LAYOUT_MEMBERS() \
    {"Flex", Py_T_OBJECT_EX, offsetof(PyLeleStyleLayout, _flex), 0, "Flex"},\
    {"Grid", Py_T_OBJECT_EX, offsetof(PyLeleStyleLayout, _grid), 0, "Grid"},\
    {"No", Py_T_OBJECT_EX, offsetof(PyLeleStyleLayout, _no), 0, "No"},\

///////////////////////////////////////////////////
struct PyLeleStyleFlow {
    PyObject ob_base;
    static PyTypeObject _obj_type;
    static PyMemberDef _members[];
    static int init(PyObject *self, PyObject *args, PyObject *kwds);
    static void dealloc(PyObject* self);
    // Type-specific fields go here
    PyObject *_row = nullptr;
    PyObject *_column = nullptr;
    PyObject *_row_wrap = nullptr;
    PyObject *_row_reverse = nullptr;
    PyObject *_row_wrap_reverse = nullptr;
    PyObject *_column_wrap = nullptr;
    PyObject *_column_reverse = nullptr;
    PyObject *_column_wrap_reverse = nullptr;
};

#define PY_LELESTYLE_FLOW_MEMBERS() \
    {"Row", Py_T_OBJECT_EX, offsetof(PyLeleStyleFlow, _row), 0, "Row"},\
    {"Column", Py_T_OBJECT_EX, offsetof(PyLeleStyleFlow, _column), 0, "Column"},\
    {"RowWrap", Py_T_OBJECT_EX, offsetof(PyLeleStyleFlow, _row_wrap), 0, "RowWrap"},\
    {"RowReverse", Py_T_OBJECT_EX, offsetof(PyLeleStyleFlow, _row_reverse), 0, "RowReverse"},\
    {"RowWrapReverse", Py_T_OBJECT_EX, offsetof(PyLeleStyleFlow, _row_wrap_reverse), 0, "RowWrapReverse"},\
    {"ColumnWrap", Py_T_OBJECT_EX, offsetof(PyLeleStyleFlow, _column_wrap), 0, "ColumnWrap"},\
    {"ColumnReverse", Py_T_OBJECT_EX, offsetof(PyLeleStyleFlow, _column_reverse), 0, "ColumnReverse"},\
    {"ColumnWrapReverse", Py_T_OBJECT_EX, offsetof(PyLeleStyleFlow, _column_wrap_reverse), 0, "ColumnWrapReverse"},\

///////////////////////////////////////////////////
struct PyLeleStyleScrollbar {
    PyObject ob_base;
    static PyTypeObject _obj_type;
    static PyMemberDef _members[];
    static int init(PyObject *self, PyObject *args, PyObject *kwds);
    static void dealloc(PyObject* self);
    // Type-specific fields go here
    PyObject *_off = nullptr;
    PyObject *_on = nullptr;
    PyObject *_active = nullptr;
    PyObject *_auto = nullptr;
};

#define PY_LELESTYLE_SCROLLBAR_MEMBERS() \
    {"Off", Py_T_OBJECT_EX, offsetof(PyLeleStyleScrollbar, _off), 0, "Off"},\
    {"On", Py_T_OBJECT_EX, offsetof(PyLeleStyleScrollbar, _on), 0, "On"},\
    {"Active", Py_T_OBJECT_EX, offsetof(PyLeleStyleScrollbar, _active), 0, "Active"},\
    {"Auto", Py_T_OBJECT_EX, offsetof(PyLeleStyleScrollbar, _auto), 0, "Auto"},\

///////////////////////////////////////////////////
struct PyLeleStyleBorder {
    PyObject ob_base;
    static PyTypeObject _obj_type;
    static PyMemberDef _members[];
    static int init(PyObject *self, PyObject *args, PyObject *kwds);
    static void dealloc(PyObject* self);
    // Type-specific fields go here
    PyObject *_no = nullptr;
    PyObject *_solid = nullptr;
    PyObject *_dashed = nullptr;
    PyObject *_dotted = nullptr;
};

#define PY_LELESTYLE_BORDER_MEMBERS() \
    {"No", Py_T_OBJECT_EX, offsetof(PyLeleStyleBorder, _no), 0, "No"},\
    {"Solid", Py_T_OBJECT_EX, offsetof(PyLeleStyleBorder, _solid), 0, "Solid"},\
    {"Dashed", Py_T_OBJECT_EX, offsetof(PyLeleStyleBorder, _dashed), 0, "Dashed"},\
    {"Dotted", Py_T_OBJECT_EX, offsetof(PyLeleStyleBorder, _dotted), 0, "Dotted"},\

