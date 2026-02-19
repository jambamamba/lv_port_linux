#pragma once

#include <functional>
#include <string>

std::string tr(const std::string &txt);
void lele_set_translatable_text(std::function<void()> set_text_func);

#pragma once

#include <lvgl/lvgl.h>
#include <Python.h>
#include <string>
#include <vector>
#include <unordered_map>

class LeleLanguage {
public:
  PyObject *createPyObject();
  static LeleLanguage &getLeleLanguage();

  const std::unordered_map<std::string, std::string> &getAvailableLanguages();
  std::string getCurrentLanguage();
  bool setCurrentLanguage(const std::string &language);
  std::string getDefaultLanguage();
  bool setDefaultLanguage(const std::string &language);

protected:
  LeleLanguage() = default;
//   mutable std::unordered_map<std::string, std::unordered_map<int, Font>> _font_db;
};

struct PyLeleLanguage {
    PyObject ob_base;
    static PyTypeObject _obj_type;
    static PyMemberDef _members[];
    static PyMethodDef _methods[];
    static void dealloc(PyObject* self);
    static int init(PyObject *self, PyObject *args, PyObject *kwds);
    // Type-specific fields go here
    LeleLanguage *_lele_translation = nullptr;
    static PyObject *getAvailableLanguages(PyObject *self_, PyObject *args);
    static PyObject *getCurrentLanguage(PyObject *self_, PyObject *args);
    static PyObject *setCurrentLanguage(PyObject *self_, PyObject *args);
    static PyObject *getDefaultLanguage(PyObject *self_, PyObject *args);
    static PyObject *setDefaultLanguage(PyObject *self_, PyObject *args);
};

#define PY_LELELANGUAGE_MEMBERS() \

#define PY_LELELANGUAGE_METHODS() \
  {"getAvailableLanguages", (PyCFunction)PyLeleLanguage::getAvailableLanguages, METH_NOARGS, "Get the available languages"},\
  {"getCurrentLanguage", (PyCFunction)PyLeleLanguage::getCurrentLanguage, METH_NOARGS, "Get the current language"},\
  {"setCurrentLanguage", (PyCFunction)PyLeleLanguage::setCurrentLanguage, METH_VARARGS, "Set the current language"},\
  {"getDefaultLanguage", (PyCFunction)PyLeleLanguage::getDefaultLanguage, METH_NOARGS, "Get the default language"},\
  {"setDefaultLanguage", (PyCFunction)PyLeleLanguage::setDefaultLanguage, METH_VARARGS, "Set the default language"},\

