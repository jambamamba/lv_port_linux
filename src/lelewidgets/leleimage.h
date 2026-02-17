#pragma once

#include "leleobject.h"

#include <map>
#include <optional>
#include <image_builder/image_builder.h>
#include <lv_image_converter/lv_image_converter.h>

class LeleImage : public LeleObject  {
  public:
  LeleImage(LeleObject *parent, const std::string &json_str);
  virtual bool fromJson(const std::string &json_str) override;
  virtual lv_obj_t *createLvObj(LeleObject *lele_parent = nullptr, lv_obj_t *lv_obj = nullptr) override;
  virtual PyObject *createPyObject() override;
  virtual bool initPyObject(PyLeleObject *py_obj) override;
  std::string getSrc() const;
  void setSrc(const std::string& src);
  std::vector<uint8_t> getBuffer() const;
  void setBuffer(const std::vector<uint8_t>& buffer);
  std::tuple<int,int> getSize() const;
  void setSize(int width, int height);
  std::tuple<int,int> getPosition() const;
  void setPosition(int width, int height);
  std::tuple<float, int,int> getRotation() const;
  void setRotation(float angle, int pivot_x, int pivot_y);

  protected:
  void drawImage();
  
  lv_obj_t *_lv_img = nullptr;
  ImageBuilder::Res _img;
  std::map<std::string, std::optional<LeleStyle::StyleValue>> _img_style;
  std::vector<std::string> _attributes_as_ordered_in_json;
};

struct PyLeleImage {
    PyLeleObject ob_base;
    static PyTypeObject _obj_type;
    static PyMemberDef _members[];
    static PyMethodDef _methods[];
    static void dealloc(PyObject* self);
    static int init(PyObject *self, PyObject *args, PyObject *kwds);
    // Type-specific fields go here
    static PyObject *getSrc(PyObject *, PyObject *);
    static PyObject *setSrc(PyObject *, PyObject *);
    static PyObject *getBuffer(PyObject *, PyObject *);
    static PyObject *setBuffer(PyObject *, PyObject *);
    static PyObject *getSize(PyObject *, PyObject *);
    static PyObject *setSize(PyObject *, PyObject *);
    static PyObject *getPosition(PyObject *, PyObject *);
    static PyObject *setPosition(PyObject *, PyObject *);
    static PyObject *getRotation(PyObject *, PyObject *);
    static PyObject *setRotation(PyObject *, PyObject *);
};

#define PY_LELEIMAGE_MEMBERS() \
  PY_LELEOBJECT_MEMBERS()

#define PY_LELEIMAGE_METHODS() \
  PY_LELEOBJECT_METHODS() \
  {"getSrc", (PyCFunction)PyLeleImage::getSrc, METH_NOARGS, "Get the image source"},\
  {"setSrc", (PyCFunction)PyLeleImage::setSrc, METH_VARARGS, "Set the image source"},\
  {"getBuffer", (PyCFunction)PyLeleImage::getBuffer, METH_NOARGS, "Get the image buffer"},\
  {"setBuffer", (PyCFunction)PyLeleImage::setBuffer, METH_VARARGS, "Set the image buffer"},\
  {"getSize", (PyCFunction)PyLeleImage::getSize, METH_NOARGS, "Get the image size as tuple x,y"},\
  {"setSize", (PyCFunction)PyLeleImage::setSize, METH_VARARGS, "Set the image size with tuple x,y"},\
  {"getPosition", (PyCFunction)PyLeleImage::getPosition, METH_NOARGS, "Get the image position"},\
  {"setPosition", (PyCFunction)PyLeleImage::setPosition, METH_VARARGS, "Set the image position"},\
  {"getRotation", (PyCFunction)PyLeleImage::getRotation, METH_NOARGS, "Get the image rotation"},\
  {"setRotation", (PyCFunction)PyLeleImage::setRotation, METH_VARARGS, "Set the image rotation"},\

