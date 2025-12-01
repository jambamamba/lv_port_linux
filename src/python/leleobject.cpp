#include <lelewidgets/leleobject.h>

static void
PyLeleObject_dealloc(PyLeleObject* self) {
    Py_XDECREF(self->_id);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject *
PyLeleObject_new(PyTypeObject *type, const LeleObject *lele_object) {
    PyLeleObject *self = (PyLeleObject *)type->tp_alloc(type, 0);
    if (self != nullptr) {
        self->_id = PyUnicode_FromString(
            lele_object ? 
                lele_object->id().size() ? lele_object->id().c_str() : "" : 
                "");
        if (self->_id == nullptr) {
            Py_DECREF(self);
            return nullptr;
        }
    }
    return (PyObject *)self;
}

static PyObject *
PyLeleObject_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    return PyLeleObject_new(type);
}

static int
PyLeleObject_init(PyLeleObject *self, PyObject *args, PyObject *kwds) {
    self->_id = PyUnicode_FromString("id");
    return 0;
}

static PyMemberDef PyLeleObject_members[] = {
    {"id", Py_T_OBJECT_EX, offsetof(PyLeleObject, _id), 0, "id"},
    {nullptr}  /* Sentinel */
};

// static PyObject *
// PyLeleEvent_id(PyLeleEvent* self) {
//     if (self->_id == nullptr) {
//         PyErr_SetString(PyExc_AttributeError, "id");
//         return nullptr;
//     }
//     return PyUnicode_FromFormat("%S", self->_id);
// }

// static PyMethodDef PyLeleEvent_methods[] = {
//     {"id", (PyCFunction)PyLeleEvent_id, METH_NOARGS, "Return the id"},
//     {"type", (PyCFunction)PyLeleEvent_id, METH_NOARGS, "Return the type"},
//     {"action", (PyCFunction)PyLeleEvent_id, METH_NOARGS, "Return the action"},
//     {"args", (PyCFunction)PyLeleEvent_id, METH_NOARGS, "Return the args"},
//     {nullptr}  /* Sentinel */
// };

PyTypeObject PyLeleObject_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "lele.Object",             /* tp_name */
    sizeof(PyLeleObject), /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)PyLeleObject_dealloc, /* tp_dealloc */
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
    0,//PyLeleObject_methods,             /* tp_methods */
    PyLeleObject_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyLeleObject_init,      /* tp_init */
    0,                         /* tp_alloc */
    PyLeleObject_new,                 /* tp_new */
};
