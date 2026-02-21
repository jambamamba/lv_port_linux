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
  std::string getSelectedItem() const;
  void setSelectedItem(const std::string &value);
  int getSelectedItemIndex() const;
  void setSelectedItemIndex(int idx);
  int getNumberOfVisibleItems() const;
  void setNumberOfVisibleItems(int num_visible_items);
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
    static PyObject *getSelectedItem(PyObject *, PyObject *);
    static PyObject *setSelectedItem(PyObject *, PyObject *);
    static PyObject *getSelectedItemIndex(PyObject *, PyObject *);
    static PyObject *setSelectedItemIndex(PyObject *, PyObject *);
    static PyObject *getNumberOfVisibleItems(PyObject *, PyObject *);
    static PyObject *setNumberOfVisibleItems(PyObject *, PyObject *);
    static PyObject *onValueChanged(PyObject *, PyObject *);
};

#define PY_LELEROLLERVIEW_MEMBERS() \
  PY_LELEOBJECT_MEMBERS()

#define PY_LELEROLLERVIEW_METHODS() \
  PY_LELEOBJECT_METHODS() \
  {"getItems", (PyCFunction)PyLeleRollerView::getItems, METH_NOARGS, "Get the items"},\
  {"setItems", (PyCFunction)PyLeleRollerView::setItems, METH_VARARGS, "Set the items"},\
  {"getSelectedItem", (PyCFunction)PyLeleRollerView::getSelectedItem, METH_NOARGS, "Get the selected item by value"},\
  {"setSelectedItem", (PyCFunction)PyLeleRollerView::setSelectedItem, METH_VARARGS, "Set the selected item by value"},\
  {"getSelectedItemIndex", (PyCFunction)PyLeleRollerView::getSelectedItemIndex, METH_NOARGS, "Get the selected item by index"},\
  {"setSelectedItemIndex", (PyCFunction)PyLeleRollerView::setSelectedItemIndex, METH_VARARGS, "Set the selected item index"},\
  {"getNumberOfVisibleItems", (PyCFunction)PyLeleRollerView::getNumberOfVisibleItems, METH_NOARGS, "Get the number of visible items"},\
  {"setNumberOfVisibleItems", (PyCFunction)PyLeleRollerView::setNumberOfVisibleItems, METH_VARARGS, "Set the number of visible items"},\
  {"onValueChanged", (PyCFunction)PyLeleRollerView::onValueChanged, METH_VARARGS, "Sets the event handler that is triggered when value changes"},\

