#pragma once

#include "leleobject.h"

struct PyLeleColorWheel;
class LeleColorWheel : public LeleObject  {
  public:
  LeleColorWheel(const LeleObject *parent, const std::string &json_str = "");
  virtual bool fromJson(const std::string &json_str) override;
  virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  virtual PyObject *createPyObject() override;
  virtual bool initPyObject(PyLeleObject *py_obj) override;
  void setColor(int32_t rgb);
  int32_t getColor() const;
  void onColorChanged(PyObject *callback);
protected:
  std::unique_ptr<lv_color_t[]> _canvas_buffer;
  int32_t _rgb = 0;
  int _width = 0;
  int _height = 0;

  std::pair<int,int> initCanvas();
  bool pyCallback(PyObject *py_callback, int32_t rgb);
  bool eventCallback(LeleEvent &&e) override;
};

struct PyLeleColorWheel {
    PyLeleObject ob_base;
    static PyTypeObject _obj_type;
    static PyMemberDef _members[];
    static PyMethodDef _methods[];
    static void dealloc(PyObject* self);
    static int init(PyObject *self, PyObject *args, PyObject *kwds);
    // Type-specific fields go here
    static PyObject *getColor(PyObject *, PyObject *);
    static PyObject *setColor(PyObject *, PyObject *);
    static PyObject *onColorChanged(PyObject *, PyObject *);    
};

#define PY_LELECOLORWHEEL_MEMBERS() \
  PY_LELEOBJECT_MEMBERS()

#define PY_LELECOLORWHEEL_METHODS() \
  PY_LELEOBJECT_METHODS() \
  {"getColor", (PyCFunction)PyLeleColorWheel::getColor, METH_NOARGS, "Get the color"},\
  {"setColor", (PyCFunction)PyLeleColorWheel::setColor, METH_VARARGS, "Set the color"},\
  {"onColorChanged", (PyCFunction)PyLeleColorWheel::onColorChanged, METH_VARARGS, "Sets the event handler that is triggered when color changes"},\

