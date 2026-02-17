#pragma once

#include <string>

std::string tr(const std::string &txt);

#pragma once

#include <lvgl/lvgl.h>
#include <Python.h>
#include <string>
#include <vector>
#include <unordered_map>

class LeleTranslation {
public:
  PyObject *createPyObject();
  static LeleTranslation &getLeleTranslation();

  const std::unordered_map<std::string, std::string> &getAvailableLanguages();
  std::string getCurrentLanguage();
  bool setCurrentLanguage(const std::string &language);
  std::string getDefaultLanguage();
  bool setDefaultLanguage(const std::string &language);

protected:
  LeleTranslation() = default;
//   mutable std::unordered_map<std::string, std::unordered_map<int, Font>> _font_db;
};

struct PyLeleTranslation {
    PyObject ob_base;
    static PyTypeObject _obj_type;
    static PyMemberDef _members[];
    static PyMethodDef _methods[];
    static void dealloc(PyObject* self);
    static int init(PyObject *self, PyObject *args, PyObject *kwds);
    // Type-specific fields go here
    LeleTranslation *_lele_translation = nullptr;
    static PyObject *getAvailableLanguages(PyObject *self_, PyObject *args);
    static PyObject *getCurrentLanguage(PyObject *self_, PyObject *args);
    static PyObject *setCurrentLanguage(PyObject *self_, PyObject *args);
    static PyObject *getDefaultLanguage(PyObject *self_, PyObject *args);
    static PyObject *setDefaultLanguage(PyObject *self_, PyObject *args);
};

#define PY_LELETRANSLATION_MEMBERS() \

#define PY_LELETRANSLATION_METHODS() \
  {"getAvailableLanguages", (PyCFunction)PyLeleTranslation::getAvailableLanguages, METH_NOARGS, "Get the available languages"},\
  {"getCurrentLanguage", (PyCFunction)PyLeleTranslation::getCurrentLanguage, METH_NOARGS, "Get the current language"},\
  {"setCurrentLanguage", (PyCFunction)PyLeleTranslation::setCurrentLanguage, METH_VARARGS, "Set the current language"},\
  {"getDefaultLanguage", (PyCFunction)PyLeleTranslation::getDefaultLanguage, METH_NOARGS, "Get the default language"},\
  {"setDefaultLanguage", (PyCFunction)PyLeleTranslation::setDefaultLanguage, METH_VARARGS, "Set the default language"},\

