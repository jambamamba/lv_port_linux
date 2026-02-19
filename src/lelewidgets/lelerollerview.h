#pragma once

#include "leleobject.h"

struct PyLeleRollerView;
class LeleRollerView : public LeleObject  {
  public:
  LeleRollerView(LeleObject *parent, const std::string &json_str = "");
  virtual bool fromJson(const std::string &json_str) override;
  virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  virtual PyObject *createPyObject() override;
  virtual bool initPyObject(PyLeleObject *py_obj) override;
  void setText(const std::string &text);
  std::string getText() const;
  protected:
  std::string _text;

  bool eventCallback(LeleEvent &&e) override;
};

struct PyLeleRollerView {
    PyLeleObject ob_base;
    static PyTypeObject _obj_type;
    static PyMemberDef _members[];
    static PyMethodDef _methods[];
    static void dealloc(PyObject* self);
    static int init(PyObject *self, PyObject *args, PyObject *kwds);
    // Type-specific fields go here
    static PyObject *getText(PyObject *, PyObject *);
    static PyObject *setText(PyObject *, PyObject *);
};

#define PY_LELEROLLERVIEW_MEMBERS() \
  PY_LELEOBJECT_MEMBERS()

#define PY_LELEROLLERVIEW_METHODS() \
  PY_LELEOBJECT_METHODS() \
  {"getText", (PyCFunction)PyLeleRollerView::getText, METH_NOARGS, "Get the text"},\
  {"setText", (PyCFunction)PyLeleRollerView::setText, METH_VARARGS, "Set the text"},

