#include <lelewidgets/lelelabel.h>

PyObject *PyLeleLabel::createPyObject(LeleObject *lele_object) {
    PyTypeObject *type = &PyLeleLabel::_obj_type;
    PyType_Ready(type);
    PyLeleLabel *self = (PyLeleLabel *)type->tp_alloc(type, 0);
    if (self != nullptr) {
        if(lele_object && lele_object->className() == "LeleLabel") {
            LeleLabel *lele_label = dynamic_cast<LeleLabel *>(lele_object);
            self->_text = PyUnicode_FromString(lele_label->getText().c_str());
        }
        else {
            self->_text = PyUnicode_FromString("");
        }
        if (self->_text == nullptr) {
            Py_DECREF(self);
            return nullptr;
        }
    }
    return (PyObject *)self;
}

int PyLeleLabel::init(PyObject *self_, PyObject *args, PyObject *kwds) {
    PyLeleLabel *self = reinterpret_cast<PyLeleLabel *>(self_);
    self->_text = PyUnicode_FromString("text");
    return 0;
}

void PyLeleLabel::dealloc(PyObject* self_) {
    PyLeleLabel *self = reinterpret_cast<PyLeleLabel *>(self_);
    Py_XDECREF(self->_text);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyMemberDef PyLeleLabel::_members[] = {
    PY_LELEOBJECT_MEMBERS()
    PY_LELELABEL_MEMBERS()
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

// static PyMethodDef PyLeleEvent::methods[] = {
//     {"id", (PyCFunction)PyLeleEvent_id, METH_NOARGS, "Return the id"},
//     {"type", (PyCFunction)PyLeleEvent_id, METH_NOARGS, "Return the type"},
//     {"action", (PyCFunction)PyLeleEvent_id, METH_NOARGS, "Return the action"},
//     {"args", (PyCFunction)PyLeleEvent_id, METH_NOARGS, "Return the args"},
//     {nullptr}  /* Sentinel */
// };

PyTypeObject PyLeleLabel::_obj_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "lele.Label",             /* tp_name */
    sizeof(PyLeleLabel), /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)PyLeleLabel::dealloc, /* tp_dealloc */
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
    0,//PyLeleLabel::_methods,             /* tp_methods */
    PyLeleLabel::_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyLeleLabel::init,      /* tp_init */
    0,                         /* tp_alloc */
    PyType_GenericNew,                 /* tp_new */
};
