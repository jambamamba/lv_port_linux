#include "tr.h"

PyObject *LeleLanguage::createPyObject() {
    
    // LOG(DEBUG, LVSIM, "createPyObject\n");
    PyTypeObject *type = &PyLeleLanguage::_obj_type;
    PyType_Ready(type);
    PyLeleLanguage *self = (PyLeleLanguage *)type->tp_alloc(type, 0);
    if (!self) {
        return Py_None;
    }
    self->_lele_translation = this;
    return (PyObject *)self;
}

int PyLeleLanguage::init(PyObject *self_, PyObject *args, PyObject *kwds) {
    PyLeleLanguage *self = reinterpret_cast<PyLeleLanguage *>(self_);
    return 0;
}

void PyLeleLanguage::dealloc(PyObject* self) {
    Py_TYPE(self)->tp_free((PyObject*)self);
}

namespace {

PyObject *toPyDict(const std::unordered_map<std::string, std::string> &languages) {

    struct RAII {
        PyObject *_dict = nullptr;
        std::unordered_map<PyObject*, PyObject*> _items;
        void reset() {
            _dict = nullptr;//this will prevent ref count decrement
        }
        ~RAII() {            
            for(const auto &[name, value] : _items) {
                Py_XDECREF(name);
                Py_XDECREF(value);
            }
            Py_XDECREF(_dict);
        }
    }$;

    $._dict = PyDict_New();
    if(!$._dict) {
        return Py_None;
    }
    for(const auto &[lang_code, lang] : languages) {

        PyObject *py_name = PyUnicode_FromString(lang_code.c_str());
        if(!py_name) {
            break;
        }
        PyObject *py_value = 
            PyUnicode_FromString(lang.c_str());
        // PyLong_FromLong(std::get<int>(style.value()));

        if(!py_value) {
            Py_XDECREF(py_name);
            break;
        }
        if(PyDict_SetItem($._dict, py_name, py_value) == -1) {
            Py_XDECREF(py_name);
            Py_XDECREF(py_value);
            break;
        }
        $._items[py_name] = py_value;
    }
    PyObject *dict = $._dict;
    $.reset();
    return dict;
}
}//namespace

PyObject *PyLeleLanguage::getAvailableLanguages(PyObject *self_, PyObject *args) {
    PyLeleLanguage *self = reinterpret_cast<PyLeleLanguage *>(self_);
    LeleLanguage *lele_translation = dynamic_cast<LeleLanguage *>(self->_lele_translation);
    if(!lele_translation) {
        return Py_None;
    }
    const auto &languages = lele_translation->getAvailableLanguages();
    return toPyDict(languages);
}

PyObject *PyLeleLanguage::getCurrentLanguage(PyObject *self_, PyObject *args) {
    PyLeleLanguage *self = reinterpret_cast<PyLeleLanguage *>(self_);
    LeleLanguage *lele_translation = dynamic_cast<LeleLanguage *>(self->_lele_translation);
    if(!lele_translation) {
        return Py_None;
    }
    std::string current_language = lele_translation->getCurrentLanguage();
    return PyUnicode_FromString(current_language.c_str());
}

PyObject *PyLeleLanguage::setCurrentLanguage(PyObject *self_, PyObject *args) {
    PyLeleLanguage *self = reinterpret_cast<PyLeleLanguage *>(self_);
    LeleLanguage *lele_translation = dynamic_cast<LeleLanguage *>(self->_lele_translation);
    if(!lele_translation) {
        return Py_None;
    }
    const char *language;
    if (!PyArg_ParseTuple(args, "s", &language)) {
        return Py_None;
    }
    bool ret = lele_translation->setCurrentLanguage(language);
    if(ret) {
        Py_RETURN_TRUE;
    }
    else {
        Py_RETURN_FALSE;
    }
}

PyObject *PyLeleLanguage::getDefaultLanguage(PyObject *self_, PyObject *args) {
    PyLeleLanguage *self = reinterpret_cast<PyLeleLanguage *>(self_);
    LeleLanguage *lele_translation = dynamic_cast<LeleLanguage *>(self->_lele_translation);
    if(!lele_translation) {
        return Py_None;
    }
    std::string default_language = lele_translation->getDefaultLanguage();
    return PyUnicode_FromString(default_language.c_str());
}

PyObject *PyLeleLanguage::setDefaultLanguage(PyObject *self_, PyObject *args) {
    PyLeleLanguage *self = reinterpret_cast<PyLeleLanguage *>(self_);
    LeleLanguage *lele_translation = dynamic_cast<LeleLanguage *>(self->_lele_translation);
    if(!lele_translation) {
        return Py_None;
    }
    const char *language;
    if (!PyArg_ParseTuple(args, "s", &language)) {
        return Py_None;
    }
    bool ret = lele_translation->setDefaultLanguage(language);
    if(ret) {
        Py_RETURN_TRUE;
    }
    else {
        Py_RETURN_FALSE;
    }
}

PyMemberDef PyLeleLanguage::_members[] = {
    PY_LELELANGUAGE_MEMBERS()
    {nullptr}  /* Sentinel */
};

PyMethodDef PyLeleLanguage::_methods[] = {
    PY_LELELANGUAGE_METHODS()
    {nullptr}  /* Sentinel */
};

static PyObject *
PyType_New(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    return LeleLanguage::getLeleLanguage().createPyObject();
}

PyTypeObject PyLeleLanguage::_obj_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "lele.Language",             /* tp_name */
    sizeof(PyLeleLanguage), /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)PyLeleLanguage::dealloc, /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /* tp_flags */
    "Language",            /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyLeleLanguage::_methods,             /* tp_methods */
    PyLeleLanguage::_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyLeleLanguage::init,      /* tp_init */
    0,                         /* tp_alloc */
    PyType_New,                 /* tp_new */
};
