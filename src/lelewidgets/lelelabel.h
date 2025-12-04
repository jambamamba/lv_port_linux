#pragma once

#include "leleobject.h"

class LeleLabel : public LeleObject  {
  public:
  LeleLabel(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  virtual PyObject *createPyObject() override;
  void setText(const std::string &text);
  std::string getText() const;
  protected:
  std::string _text;
};


struct PyLeleLabel {
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

#define PY_LELELABEL_MEMBERS() \
  {nullptr},\

#define PY_LELELABEL_METHODS() \
  {"getText", (PyCFunction)PyLeleLabel::getText, METH_NOARGS, "Get the text"},\
  {"setText", (PyCFunction)PyLeleLabel::setText, METH_VARARGS, "Set the text"},\

