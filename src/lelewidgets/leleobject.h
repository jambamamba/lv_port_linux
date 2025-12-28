#pragma once

#include "lelewidgetfactory.h"

#include <json_utils/json_utils.h>
#include <lv_image_converter/lv_image_converter.h>
#include <lvgl/lvgl_private.h>
#include <optional>
#include <typeinfo>

//need to include all the types in   using Node = std::variant<
#include "leleevent.h"
#include "lelestyle.h"

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN && LV_MEM_SIZE < (38ul * 1024ul)
    #error Insufficient memory for lv_demo_widgets. Please set LV_MEM_SIZE to at least 38KB (38ul * 1024ul).  48KB is recommended.
#endif

PyObject *getPyModule();
PyObject *getEnumModule();

typedef enum {
    DISP_SMALL,
    DISP_MEDIUM,
    DISP_LARGE,
} disp_size_t;

struct _object;
struct PyLeleObject;
typedef struct _object PyObject;
class LeleObject {
public:
  LeleObject(const std::string &json_str = "");
  virtual ~LeleObject();
  friend std::ostream& operator<<(std::ostream& os, const LeleObject& p);

  const std::string &className() const;
  const std::string &id() const;

  std::vector<LeleObject *> getLeleObj(const std::string &obj_name) const;
  bool enabled() const;
  lv_obj_t *getLvObj() const;
  void setLvObj(lv_obj_t *obj);
  void setParent(LeleObject *parent);
  LeleObject *getParent() const;
  std::vector<LeleStyle *> getStyles() const;
  std::optional<LeleStyle::StyleValue> getStyle(const std::string &key, const std::string &class_name = "") const;
  std::map<std::string, std::optional<LeleStyle::StyleValue>> getStyleAttributes(const std::string &style_id = "") const;
  std::tuple<std::vector<std::string> ,std::map<std::string, std::optional<LeleStyle::StyleValue>>> getBackgroundStyle(const std::string &class_name = "") const;
  std::vector<std::pair<std::string, LeleWidgetFactory::Node>> &children();
  virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr);
  virtual PyObject *createPyObject();
  virtual bool initPyObject(PyLeleObject *py_obj);
  virtual bool fromJson(const std::string &json_str);
  virtual void setStyle(lv_obj_t *lv_obj);
  virtual void addStyle(std::vector<std::unique_ptr<LeleStyle>> &lele_styles);
  virtual void addStyle(LeleStyle* lele_style);
  virtual void removeStyle(const std::string &style_id);
  virtual void setObjAlignStyle(lv_obj_t *lv_obj);
  virtual void setTextAlignStyle(lv_obj_t *lv_obj);
  virtual void show();
  virtual void hide();
  static void EventCallback(lv_event_t *e);
  virtual bool eventCallback(LeleEvent &&e);
  void addEventHandler(PyObject *callback);
  static bool pyCallback(PyObject *py_callback, LeleEvent &&e);
#ifdef METHOD1_CREATING_PYENUM
  static PyObject* createPyEnum(const std::string &enum_name, const std::map<std::string,int> &&enum_map);
  static PyObject* getPyEnumValue(const std::string &enum_value = "lele.Event.Clicked");
#endif
#ifdef METHOD2_CREATING_PYENUM
  static PyObject* createPyEnum(const std::string &enum_name, const std::map<std::string,int> &&enum_map);
#endif
protected:
  void drawBackgroundImage(std::optional<LeleStyle::StyleValue> value, int obj_width, int obj_height);
  std::tuple<int,int> parseBackgroundPosition(
    const std::optional<LeleStyle::StyleValue> &value, int container_width, int container_height) const;

  std::string _class_name = "N/A";
  std::string _id;
  bool _enabled = true;
  lv_obj_t *_lv_obj = nullptr;
  lv_obj_t *_lv_bg_img = nullptr;
  LeleObject *_lele_parent = nullptr;
  lv_style_t _style = {0};
  std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> _bg_img;
  std::vector<std::pair<std::string, LeleWidgetFactory::Node>> _nodes;
  // LeleStyles _lele_styles;
  std::vector<LeleStyle *>_lele_styles;
  std::vector<PyObject *> _py_callbacks;
};

struct PyLeleObject {
    PyObject ob_base;
    static PyTypeObject _obj_type;
    static PyMemberDef _members[];
    static PyMethodDef _methods[];
    static void dealloc(PyObject* self);
    static int init(PyObject *self, PyObject *args, PyObject *kwds);
    static PyObject *pyListOrPyObjectFromStdVector(const std::vector<PyObject*> &py_objects);
    // Type-specific fields go here
    LeleObject *_lele_obj = nullptr;
    PyObject *_id = nullptr;
    PyObject *_class_name = nullptr;
    static PyObject *addChild(PyObject *, PyObject *);
    static PyObject *fromConfig(PyObject *, PyObject *);
    static PyObject *getClassName(PyObject *, PyObject *);
    static PyObject *addEventHandler(PyObject *, PyObject *);
    static PyObject *getStyle(PyObject *, PyObject *);
    static PyObject *addStyle(PyObject *, PyObject *);
    static PyObject *removeStyle(PyObject *, PyObject *);
};

#define PY_LELEOBJECT_MEMBERS() \
  {"id", Py_T_OBJECT_EX, offsetof(PyLeleObject, _id), 0, "id"},

#define PY_LELEOBJECT_METHODS() \
  {"addChild", (PyCFunction)PyLeleObject::addChild, METH_VARARGS, "Adds one more more children from a configuration file with JSON description of the object"},\
  {"fromConfig", (PyCFunction)PyLeleObject::fromConfig, METH_VARARGS, "Parent object. Json config file: The object is loaded from a configuration file with JSON description of the object"},\
  {"getClassName", (PyCFunction)PyLeleObject::getClassName, METH_NOARGS, "Get the class name"},\
  {"addEventHandler", (PyCFunction)PyLeleObject::addEventHandler, METH_VARARGS, "Sets the event handler"},\
  {"getStyle", (PyCFunction)PyLeleObject::getStyle, METH_VARARGS, "Get the style attributes for this object"},\
  {"addStyle", (PyCFunction)PyLeleObject::addStyle, METH_VARARGS, "Add a style to the object"},\
  {"removeStyle", (PyCFunction)PyLeleObject::removeStyle, METH_VARARGS, "Remove a style by its given id from the object"},\

