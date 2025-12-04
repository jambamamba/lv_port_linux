#pragma once

#include "lelelabel.h"

class LeleGroup : public LeleObject {
  public:
  LeleGroup(const std::string &json_str);
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
    LeleButton(const std::string &json_str);
    virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
    virtual PyObject *createPyObject() override;
    virtual bool eventCallback(LeleEvent &&e) override;
    bool isCheckable() const { return _checkable; }
    bool isChecked() const { return _checked; }
    void setChecked(bool checked) { _checked = checked; }
    Type getType() const { return _type; }
    const std::string &text() const { return _text; }
    protected:
    bool _checkable = false;
    bool _checked = false;
    Type _type = Push;
    int _value = 0;
    std::vector<LeleEvent*> _events;
  };
  LeleButtons(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  int count() const;
  LeleButton* getAt(int idx) const;
  protected:
};


struct PyLeleButton {
    PyLeleObject ob_base;
    static PyTypeObject _obj_type;
    static PyMemberDef _members[];
    static PyMethodDef _methods[];
    static void dealloc(PyObject* self);
    static int init(PyObject *self, PyObject *args, PyObject *kwds);
    // Type-specific fields go here
    static PyObject *isCheckable(PyObject *, PyObject *);
    static PyObject *isChecked(PyObject *, PyObject *);
    static PyObject *setChecked(PyObject *, PyObject *);
    static PyObject *getType(PyObject *, PyObject *);
};

#define PY_LELEBUTTON_MEMBERS() \
  {nullptr},\

#define PY_LELEBUTTON_METHODS() \
  {"isCheckable", (PyCFunction)PyLeleButton::isCheckable, METH_NOARGS, "Is the button checkable?"},\
  {"isChecked", (PyCFunction)PyLeleButton::isChecked, METH_NOARGS, "Is the button checked?"},\
  {"setChecked", (PyCFunction)PyLeleButton::setChecked, METH_VARARGS, "Make the button checked"},\
  {"getType", (PyCFunction)PyLeleButton::getType, METH_VARARGS, "Get the type of button: Push,Checkbox,Radio,Switch,Close,Slider"},\

