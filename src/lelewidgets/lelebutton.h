#pragma once

#include "lelelabel.h"

struct PyLeleButton;
class LeleGroup : public LeleObject {
  public:
  LeleGroup(const LeleObject *parent, const std::string &json_str = "");
  virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  virtual bool eventCallback(LeleEvent &&e) override;
  protected:
  int _active_child_idx = -1;
};
class LeleButtons : public LeleObject {
  public:
  class LeleButton : public LeleLabel  {
    public:
    enum Type {
      Push,
      Checkbox,
      Radio,
      Switch,
      Close,
      Slider,
      SliderRange,
      ColorPicker,
      Arc
    };
    LeleButton(const LeleObject *parent, const std::string &json_str = "");
    virtual bool fromJson(const std::string &json_str) override;
    virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
    virtual PyObject *createPyObject() override;
    virtual bool initPyObject(PyLeleObject *py_obj) override;
    virtual bool eventCallback(LeleEvent &&e) override;
    bool click();
    bool isCheckable() const { return _checkable; }
    bool getChecked() const { return _checked; }
    void setChecked(bool checked);
    int getMin() const { return _min; }
    // void setMin(int value) { _min = value; }
    int getMax() const { return _max; }
    // void setMax(int value) { _max = value; }
    int getValue() const { return _value; }
    void setValue(int value);
    int getStartValue() const { return _start_value; }
    // void setStartValue(int value) { _start_value = value; }
    int getEndValue() const { return _end_value; }
    // void setEndValue(int value) { _end_value = value; }
    int getRotation() const { return _rotation; }
    // void setRotation(int value) { _rotation = value; }
    Type getType() const { return _type; }
    protected:
    bool _checkable = false;
    bool _checked = false;
    Type _type = Type::Push;
    int _min = 0;
    int _max = 100;
    int _value = 0;
    int _start_value = 0;//slider,arc
    int _end_value = 320;//arc
    int _rotation = 90;//arc
    std::vector<LeleEvent*> _events;
  };
  LeleButtons(const LeleObject *parent, const std::string &json_str = "");
  virtual bool fromJson(const std::string &json_str) override;
  virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  int count() const;
  LeleButton* getAt(int idx) const;
  protected:
};


struct PyLeleButton {
    PyLeleLabel ob_base;
    static PyTypeObject _obj_type;
    static PyMemberDef _members[];
    static PyMethodDef _methods[];
    static void dealloc(PyObject* self);
    static int init(PyObject *self, PyObject *args, PyObject *kwds);
    // Type-specific fields go here
    PyObject *_type = nullptr;
    static PyObject *isCheckable(PyObject *, PyObject *);
    static PyObject *getChecked(PyObject *, PyObject *);
    static PyObject *setChecked(PyObject *, PyObject *);
    static PyObject *getType(PyObject *, PyObject *);
    static PyObject *click(PyObject *, PyObject *);
    static PyObject *setValue(PyObject *, PyObject *);
    static PyObject *getValue(PyObject *, PyObject *);
    static PyObject *getMin(PyObject *, PyObject *);
    static PyObject *getMax(PyObject *, PyObject *);
    static PyObject *getStartValue(PyObject *, PyObject *);
    static PyObject *getEndValue(PyObject *, PyObject *);
    static PyObject *getRotation(PyObject *, PyObject *);
};

#define PY_LELEBUTTON_MEMBERS() \
  PY_LELELABEL_MEMBERS() \
  {"Type", Py_T_OBJECT_EX, offsetof(PyLeleButton, _type), 0, "Type"},

#define PY_LELEBUTTON_METHODS() \
  PY_LELELABEL_METHODS() \
  {"isCheckable", (PyCFunction)PyLeleButton::isCheckable, METH_NOARGS, "Is the button checkable?"},\
  {"getChecked", (PyCFunction)PyLeleButton::getChecked, METH_NOARGS, "Is the button checked?"},\
  {"setChecked", (PyCFunction)PyLeleButton::setChecked, METH_VARARGS, "Make the button checked"},\
  {"getType", (PyCFunction)PyLeleButton::getType, METH_NOARGS, "Get the type of button: Push,Checkbox,Radio,Switch,Close,Slider"},\
  {"click", (PyCFunction)PyLeleButton::click, METH_NOARGS, "Click the button"},\
  {"setValue", (PyCFunction)PyLeleButton::setValue, METH_VARARGS, "Set the value of the slider"},\
  {"getValue", (PyCFunction)PyLeleButton::getValue, METH_NOARGS, "Get the value of the slider"},\
  {"getMin", (PyCFunction)PyLeleButton::getMin, METH_NOARGS, "Get the min value of the slider or arc"},\
  {"getMax", (PyCFunction)PyLeleButton::getMax, METH_NOARGS, "Get the max value of the slider or arc"},\
  {"getStartValue", (PyCFunction)PyLeleButton::getStartValue, METH_NOARGS, "Get the start value of the slider or arc"},\
  {"getEndValue", (PyCFunction)PyLeleButton::getEndValue, METH_NOARGS, "Get the end value of the arc"},\
  {"getRotation", (PyCFunction)PyLeleButton::getRotation, METH_NOARGS, "Get the rotation value of the arc"},\

///////////////////////////////////////////////////
struct PyLeleButtonType {
    PyObject ob_base;
    static PyTypeObject _obj_type;
    static PyMemberDef _members[];
    static int init(PyObject *self, PyObject *args, PyObject *kwds);
    static void dealloc(PyObject* self);
    static PyObject *create(PyTypeObject *type, PyObject *args, PyObject *kwds);
    static int enum_traverse(PyObject *en, visitproc visit, void *arg);
    static PyObject *enum_next(PyObject *en);
    // Type-specific fields go here
    PyObject *_push = nullptr;
    PyObject *_checkbox = nullptr;
    PyObject *_radio = nullptr;
    PyObject *_switch = nullptr;
    PyObject *_close = nullptr;
    PyObject *_slider = nullptr;
};

#define PY_LELEBUTTON_TYPE_MEMBERS() \
    {"Push", Py_T_OBJECT_EX, offsetof(PyLeleButtonType, _push), 0, "Push"},\
    {"Checkbox", Py_T_OBJECT_EX, offsetof(PyLeleButtonType, _checkbox), 0, "Checkbox"},\
    {"Radio", Py_T_OBJECT_EX, offsetof(PyLeleButtonType, _radio), 0, "Radio"},\
    {"Switch", Py_T_OBJECT_EX, offsetof(PyLeleButtonType, _switch), 0, "Switch"},\
    {"Close", Py_T_OBJECT_EX, offsetof(PyLeleButtonType, _close), 0, "Close"},\
    {"Slider", Py_T_OBJECT_EX, offsetof(PyLeleButtonType, _slider), 0, "Slider"},\

