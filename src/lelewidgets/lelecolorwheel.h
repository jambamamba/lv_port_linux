#pragma once

#include "leleobject.h"

struct PyLeleColorWheel;
class LeleColorWheel : public LeleObject  {
  public:
  LeleColorWheel(const std::string &json_str = "");
  virtual bool fromJson(const std::string &json_str) override;
  virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  virtual PyObject *createPyObject() override;
  virtual bool initPyObject(PyLeleObject *py_obj) override;
protected:
  void makeColorWheel(int width, int height) const;
};

struct PyLeleColorWheel {
    PyLeleObject ob_base;
    static PyTypeObject _obj_type;
    static PyMemberDef _members[];
    static PyMethodDef _methods[];
    static void dealloc(PyObject* self);
    static int init(PyObject *self, PyObject *args, PyObject *kwds);
    // Type-specific fields go here
};

#define PY_LELECOLORWHEEL_MEMBERS() \
  PY_LELEOBJECT_MEMBERS()

#define PY_LELECOLORWHEEL_METHODS() \
  PY_LELEOBJECT_METHODS() \

