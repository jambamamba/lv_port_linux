#pragma once

#include "leleobject.h"

#include <vector>

struct PyLeleRollerView;
class LeleRollerView : public LeleObject  {
  public:
  LeleRollerView(LeleObject *parent, const std::string &json_str = "");
  virtual bool fromJson(const std::string &json_str) override;
  virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  virtual PyObject *createPyObject() override;
  virtual bool initPyObject(PyLeleObject *py_obj) override;
  void setItems(const std::vector<std::string> &itemsZ);
  std::vector<std::string> getItems() const;
  void onValueChanged(PyObject *callback);
  
  protected:
  int _num_visible_items = 4;
  std::vector<std::string> _items;
  int _max_item_len = 32;

  bool eventCallback(LeleEvent &&e) override;
  bool pyCallback(PyObject *py_callback, const std::string &value);
};

struct PyLeleRollerView {
    PyLeleObject ob_base;
    static PyTypeObject _obj_type;
    static PyMemberDef _members[];
    static PyMethodDef _methods[];
    static void dealloc(PyObject* self);
    static int init(PyObject *self, PyObject *args, PyObject *kwds);
    // Type-specific fields go here
    static PyObject *getItems(PyObject *, PyObject *);
    static PyObject *setItems(PyObject *, PyObject *);
    static PyObject *onValueChanged(PyObject *, PyObject *);
};

#define PY_LELEROLLERVIEW_MEMBERS() \
  PY_LELEOBJECT_MEMBERS()

#define PY_LELEROLLERVIEW_METHODS() \
  PY_LELEOBJECT_METHODS() \
  {"getItems", (PyCFunction)PyLeleRollerView::getItems, METH_NOARGS, "Get the text"},\
  {"setItems", (PyCFunction)PyLeleRollerView::setItems, METH_VARARGS, "Set the text"},\
  {"onValueChanged", (PyCFunction)PyLeleRollerView::onValueChanged, METH_VARARGS, "Sets the event handler that is triggered when value changes"},\

