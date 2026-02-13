#pragma once

#include <lvgl/lvgl.h>
#include <unordered_map>
#include <string>

class LeleFont {
public:
  struct Font {
    std::string _lvf_file;
    lv_font_t * _lv_font = nullptr;
    Font(const std::string &lvf_file);
    ~Font();
  };
  const lv_font_t *getFont(const std::string &family="montserrat", int size=16);
  std::unordered_map<std::string, std::unordered_map<int, Font>> getFontDb() const;
protected:
  std::unordered_map<std::string, std::unordered_map<int, Font>> _font_db;
};

struct PyLeleFont {
    PyObject ob_base;
    static PyTypeObject _obj_type;
    static PyMemberDef _members[];
    static PyMethodDef _methods[];
    static void dealloc(PyObject* self);
    static int init(PyObject *self, PyObject *args, PyObject *kwds);
    // Type-specific fields go here
    static PyObject *getFontDb(PyObject *self_, PyObject *args);
};

#define PY_LELELABEL_MEMBERS() \

#define PY_LELEFONT_METHODS() \
  {"getFontDb", (PyCFunction)PyLeleFont::getFontDb, METH_NOARGS, "Get the font database"},\

