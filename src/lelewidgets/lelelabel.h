#pragma once

#include "leleobject.h"

class LeleLabel : public LeleObject  {
  public:
  LeleLabel(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  void setText(const std::string &text);
  std::string getText() const;
  protected:
  std::string _text;
};


struct PyLeleLabel {
    PyLeleObject ob_base;
    static PyTypeObject _obj_type;
    static PyMemberDef _members[];
    // static PyMethodDef _methods[];
    static void dealloc(PyObject* self);
    static int init(PyObject *self, PyObject *args, PyObject *kwds);
    static PyObject *createPyObject(LeleObject *lele_object);
    // Type-specific fields go here
    PyObject *_text = nullptr;
};

#define PY_LELELABEL_MEMBERS() \
  {"text", Py_T_OBJECT_EX, offsetof(PyLeleLabel, _text), 0, "text"},\

