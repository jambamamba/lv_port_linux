#pragma once

#include "leleobject.h"

#include <map>
#include <optional>
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

  protected:
  void drawImage();
  
  lv_obj_t *_lv_img = nullptr;
  std::optional<AutoFreeSharedPtr<lv_image_dsc_t>> _img_dsc;
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
};

#define PY_LELEIMAGE_MEMBERS() \
  PY_LELEOBJECT_MEMBERS()

#define PY_LELEIMAGE_METHODS() \
  PY_LELEOBJECT_METHODS() \
  {"getSrc", (PyCFunction)PyLeleImage::getSrc, METH_NOARGS, "Get the image source"},\
  {"setSrc", (PyCFunction)PyLeleImage::setSrc, METH_VARARGS, "Set the image source"},

