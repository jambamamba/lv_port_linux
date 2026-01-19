#pragma once

#include "lelelabel.h"
#include "lelebutton.h"

struct PyLeleMessageBox;
class LeleEvent;
class LeleMessageBox : public LeleLabel  {
public:
  LeleMessageBox(const LeleObject *parent, const std::string &json_str = "");
  virtual bool fromJson(const std::string &json_str) override;
  virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  virtual PyObject *createPyObject() override;
  virtual bool initPyObject(PyLeleObject *py_obj) override;
  virtual bool eventCallback(LeleEvent &&e) override;
  void setTitle(const std::string &text);
  std::string getTitle() const;
  LeleButtons::LeleButton *getButtonClicked() const;
protected:
  void addEventCallback(LeleButtons::LeleButton *lele_btn, lv_obj_t *lv_btn) const;

  mutable std::string _title;
  lv_obj_t *_lv_title = nullptr;
  std::unique_ptr<LeleButtons::LeleButton> _btn;
  std::vector<LeleEvent*> _events;
  LeleButtons::LeleButton *_btn_clicked = nullptr;
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
    static PyObject *getButtonClicked(PyObject *, PyObject *);
};

#define PY_LELEMESSAGEBOX_MEMBERS() \
  PY_LELELABEL_MEMBERS()

#define PY_LELEMESSAGEBOX_METHODS() \
  PY_LELELABEL_METHODS() \
  {"getTitle", (PyCFunction)PyLeleMessageBox::getTitle, METH_NOARGS, "Get message box title"},\
  {"setTitle", (PyCFunction)PyLeleMessageBox::setTitle, METH_VARARGS, "Set message box title"},\
  {"getButtonClicked", (PyCFunction)PyLeleMessageBox::getButtonClicked, METH_NOARGS, "Get the clicked button"},\

