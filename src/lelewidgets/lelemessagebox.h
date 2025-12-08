#pragma once

#include "lelelabel.h"
#include "lelebutton.h"

struct PyLeleMessageBox;
class LeleEvent;
class LeleMessageBox : public LeleLabel  {
  public:
  LeleMessageBox(const std::string &json_str);
  virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  virtual PyObject *createPyObject() override;
  bool initPyObject(PyLeleMessageBox *py_obj);
  virtual bool eventCallback(LeleEvent &&e) override;
  void setTitle(const std::string &text);
  std::string getTitle() const;
  protected:
  mutable std::string _title;
  lv_obj_t *_lv_title = nullptr;
  std::unique_ptr<LeleButtons::LeleButton> _btn;
  std::vector<LeleEvent*> _events;
};


struct PyLeleMessageBox {
    PyLeleLabel ob_base;
    static PyTypeObject _obj_type;
    static PyMemberDef _members[];
    static PyMethodDef _methods[];
    static void dealloc(PyObject* self);
    static int init(PyObject *self, PyObject *args, PyObject *kwds);
    // Type-specific fields go here
    static PyObject *getTitle(PyObject *, PyObject *);
    static PyObject *setTitle(PyObject *, PyObject *);
};

#define PY_LELEMESSAGEBOX_MEMBERS() \
  PY_LELELABEL_MEMBERS()

#define PY_LELEMESSAGEBOX_METHODS() \
  PY_LELELABEL_METHODS() \
  {"getTitle", (PyCFunction)PyLeleMessageBox::getTitle, METH_NOARGS, "Get message box title"},\
  {"setTitle", (PyCFunction)PyLeleMessageBox::setTitle, METH_VARARGS, "Set message box title"},\

