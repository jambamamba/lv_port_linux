#pragma once

#include "lelelabel.h"

struct PyLeleButton;
class LeleGroup : public LeleObject {
  public:
  LeleGroup(const std::string &json_str = "");
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
      Slider
    };
    LeleButton(const std::string &json_str = "");
    virtual bool fromJson(const std::string &json_str) override;
    virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
    virtual PyObject *createPyObject() override;
    bool initPyObject(PyLeleButton *py_obj);
    virtual bool eventCallback(LeleEvent &&e) override;
    bool isCheckable() const { return _checkable; }
    bool getChecked() const { return _checked; }
    void setChecked(bool checked);
    bool click();
    Type getType() const { return _type; }
    int getValue() const { return _value; }
    void setValue(int value);
    protected:
    bool _checkable = false;
    bool _checked = false;
    Type _type = Type::Push;
    int _value = 0;
    std::vector<LeleEvent*> _events;
  };
  LeleButtons(const std::string &json_str = "");
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
    static PyObject *getValue(PyObject *, PyObject *);
    static PyObject *setValue(PyObject *, PyObject *);
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
  {"getValue", (PyCFunction)PyLeleButton::getChecked, METH_NOARGS, "Get the value of the slider"},\
  {"setValue", (PyCFunction)PyLeleButton::setChecked, METH_VARARGS, "Set the value of the slider"},\

