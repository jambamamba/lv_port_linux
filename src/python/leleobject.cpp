#include <lelewidgets/leleobject.h>

PyObject *LeleObject::createPyObject() {
    PyTypeObject *type = &PyLeleObject::_obj_type;
    PyType_Ready(type);
    PyLeleObject *self = (PyLeleObject *)type->tp_alloc(type, 0);
    if (self != nullptr) {
        self->_lele_obj = this;
        self->_id = PyUnicode_FromString(
            _id.size() ? _id.c_str() : "");
        if (self->_id == nullptr) {
            Py_DECREF(self);
            return nullptr;
        }
        self->_class_name = PyUnicode_FromString(
            _class_name.size() ? _class_name.c_str() : "");
        if (self->_class_name == nullptr) {
            Py_DECREF(self);
            return nullptr;
        }
    }
    return (PyObject *)self;
}

PyObject* LeleObject::createPyEnum(const std::string &enum_name, const std::map<std::string,int> &&enum_map) {//https://stackoverflow.com/a/69290003
    std::string enum_str =
    "from enum import Enum\n"
    "class ";
    enum_str += enum_name;
    enum_str += "(Enum):\n";

    for(auto &[key,value] : enum_map) {
        enum_str += "    " + key + " = " + std::to_string(value) + "\n";
    }
    enum_str += "";
    PyObject *global_dict=nullptr, *should_be_none=nullptr, *output=nullptr;
    global_dict = PyDict_New();
    if (!global_dict) goto cleanup;
    should_be_none = PyRun_String(enum_str.c_str(), Py_file_input, global_dict, global_dict);
    if (!should_be_none) goto cleanup;
    // extract Color from global_dict
    output = PyDict_GetItemString(global_dict, "Type");
    if (!output) {
        // PyDict_GetItemString does not set exceptions
        PyErr_SetString(PyExc_KeyError, "could not get 'Type'");
    } else {
        Py_INCREF(output); // PyDict_GetItemString returns a borrow reference
    }
    cleanup:
    Py_XDECREF(global_dict);
    Py_XDECREF(should_be_none);
    return output;
}


int PyLeleObject::init(PyObject *self_, PyObject *args, PyObject *kwds) {
    PyLeleObject *self = reinterpret_cast<PyLeleObject *>(self_);
    self->_id = PyUnicode_FromString("id");
    self->_class_name = PyUnicode_FromString("class_name");
    return 0;
}

void PyLeleObject::dealloc(PyObject* self_) {
    PyLeleObject *self = reinterpret_cast<PyLeleObject *>(self_);
    Py_XDECREF(self->_id);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject *PyLeleObject::getClassName(PyObject *self_, PyObject *arg) {
    PyLeleObject *self = reinterpret_cast<PyLeleObject *>(self_);
    if (!self->_class_name) {
        PyErr_SetString(PyExc_AttributeError, "class_name");
        return nullptr;
    }
    return self->_class_name;
}

PyMemberDef PyLeleObject::_members[] = {
    PY_LELEOBJECT_MEMBERS()
    {nullptr}  /* Sentinel */
};

PyMethodDef PyLeleObject::_methods[] = {
    PY_LELEOBJECT_METHODS()
    {nullptr}  /* Sentinel */
};

PyTypeObject PyLeleObject::_obj_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "lele.Object",             /* tp_name */
    sizeof(PyLeleObject), /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)PyLeleObject::dealloc, /* tp_dealloc */
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
    "Object",            /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyLeleObject::_methods,             /* tp_methods */
    PyLeleObject::_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyLeleObject::init,      /* tp_init */
    0,                         /* tp_alloc */
    PyType_GenericNew,                 /* tp_new */
};
