#pragma once


#include <debug_logger/debug_logger.h>
#include <json_utils/json_utils.h>
#include <lvgl/lvgl_private.h>
#include <map>
#include <memory>
#include <optional>
#include <string>
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
  struct Rotation {
    float _angle = 0.;
    int _pivot_x = 0;
    int _pivot_y = 0;
  };
  using StyleValue = std::variant<
    int,
    std::string,
    lv_layout_t,
    lv_flex_flow_t,
    lv_scrollbar_mode_t,
    BorderTypeE,
    LeleStyle::Rotation
  >;
  // static LeleStyle fromJson(int parent_width = 0, int parent_height = 0, const cJSON *json = nullptr);
  // LeleStyle(int parent_width = 0, int parent_height = 0, const std::string &x = "", const std::string &y = "", const std::string &width = "", const std::string &height = "");
  LeleStyle(const std::string &json_str = "", lv_obj_t *parent = lv_screen_active());
  virtual bool fromJson(const std::string &json_str);
  virtual PyObject *createPyObject();
  virtual bool initPyObject(PyLeleStyle *py_obj);
  friend std::ostream& operator<<(std::ostream& os, const LeleStyle& p);
  void setLeleParent(const LeleObject *lele_parent) { _lele_parent = lele_parent; }
  const LeleObject *getLeleParent() const { return _lele_parent; }
  static int parseColorCode(const std::string &color_str);
  static int parsePercentValue(const std::string &x, int parent_x);
  static std::optional<LeleStyle::Rotation> parseRotation(const std::string &json_str);
  static std::string trim(const std::string& str);
  static std::tuple<int,int,int,int> parsePaddingOrMargin(const std::string &padding_str);
  static std::tuple<LeleStyle::BorderTypeE,int,int> parseBorder(const std::string &border_type_width_color);
  static std::tuple<std::string,std::string,std::string,std::string> parseTopRightBottomLeft(const std::string &value);
  std::string getClassName() const;
  std::optional<StyleValue> getValue(const std::string &key, const std::string &class_name = "") const;
  bool setValue(const std::string &key, const std::string &value);
protected:
  const LeleObject *_lele_parent = nullptr;
  std::string _class_name;
  std::string _id;
  std::map<std::string, std::optional<StyleValue>> _style = {
    {"x", std::nullopt},
    {"y", std::nullopt},
    {"width", std::nullopt},
    {"height", std::nullopt},
    {"corner_radius", std::nullopt},
    {"padding/top", std::nullopt},
    {"padding/right", std::nullopt},
    {"padding/bottom", std::nullopt},
    {"padding/left", std::nullopt},
    {"margin/top", std::nullopt},
    {"margin/right", std::nullopt},
    {"margin/bottom", std::nullopt},
    {"margin/left", std::nullopt},
    {"border/type", std::nullopt},
    {"border/width", std::nullopt},
    {"border/color", std::nullopt},
    {"layout", std::nullopt},
    {"flow", std::nullopt},
    {"grow", std::nullopt},
    {"fgcolor", std::nullopt},
    {"bgcolor", std::nullopt},
    {"checked_color", std::nullopt},
    {"align", std::nullopt},
    {"background/image", std::nullopt},
    {"background/position", std::nullopt},
    {"background/size", std::nullopt},
    {"background/repeat", std::nullopt},
    {"background/color", std::nullopt},
    {"background/rotate", std::nullopt},
    {"scrollbar", std::nullopt}
  };
  int _parent_width = 0;
  int _parent_height = 0;
};
// class LeleStyles {
// public:
//   LeleStyles(const std::string &json_str = "");
//   friend std::ostream& operator<<(std::ostream& os, const LeleStyles& p);
//   // void setLeleParent(const LeleObject *lele_parent);
//   size_t size() const { return _lele_styles.size(); }
//   void addStyle(std::vector<std::unique_ptr<LeleStyle>> &lele_styles);
//   LeleStyles &operator+=(LeleStyles &);
//   LeleStyles &operator+=(LeleStyle &);
//   std::optional<LeleStyle::StyleValue> getValue(const std::string &key, std::string class_name = "") const;
//   protected:
//   std::string _id;
//   LeleStyle _null_pos;
//   std::vector<LeleStyle *>_lele_styles;
//   // const LeleObject *_lele_parent = nullptr;
// };

struct PyLeleStyle {
    PyObject ob_base;
    static PyTypeObject _obj_type;
    static PyMemberDef _members[];
    static PyMethodDef _methods[];
    static void dealloc(PyObject* self);
    static int init(PyObject *self, PyObject *args, PyObject *kwds);
    // Type-specific fields go here
    std::vector<std::unique_ptr<LeleStyle>> _lele_styles;
    LeleStyle* _lele_style = nullptr;
    PyObject *_id = nullptr;
    PyObject *_class_name = nullptr;
    static PyObject *fromConfig(PyObject *, PyObject *);
    static PyObject *getClassName(PyObject *, PyObject *);
};

#define PY_LELESTYLE_MEMBERS() \
  {"id", Py_T_OBJECT_EX, offsetof(PyLeleStyle, _id), 0, "id"},

#define PY_LELESTYLE_METHODS() \
  {"fromConfig", (PyCFunction)PyLeleStyle::fromConfig, METH_VARARGS, "Parent object. Json config file: The object is loaded from a configuration file with JSON description of the object"},\
  {"getClassName", (PyCFunction)PyLeleStyle::getClassName, METH_NOARGS, "Get the class name"},\

