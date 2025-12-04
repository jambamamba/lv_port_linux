#include <lelewidgets/lelelabel.h>

PyObject *LeleLabel::createPyObject() {
    PyTypeObject *type = &PyLeleLabel::_obj_type;
    PyType_Ready(type);
    PyLeleLabel *self = (PyLeleLabel *)type->tp_alloc(type, 0);
    if (self != nullptr) {
        self->ob_base._lele_obj = this;
    }
    return (PyObject *)self;
}

int PyLeleLabel::init(PyObject *self_, PyObject *args, PyObject *kwds) {
    // PyLeleLabel *self = reinterpret_cast<PyLeleLabel *>(self_);
    return 0;
}

void PyLeleLabel::dealloc(PyObject* self_) {
    PyLeleLabel *self = reinterpret_cast<PyLeleLabel *>(self_);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject *PyLeleLabel::getText(PyObject *self_, PyObject *arg) {
    PyLeleLabel *self = reinterpret_cast<PyLeleLabel *>(self_);
    LeleLabel *lele_obj = dynamic_cast<LeleLabel *>(self->ob_base._lele_obj);
    if(lele_obj) {
        return PyUnicode_FromString(lele_obj->getText().c_str());
    }
    return Py_None;
}

PyObject *PyLeleLabel::setText(PyObject *self_, PyObject *args) {
    PyLeleLabel *self = reinterpret_cast<PyLeleLabel *>(self_);
    LeleLabel *lele_obj = dynamic_cast<LeleLabel *>(self->ob_base._lele_obj);
    if(lele_obj && args) {
        char *str = nullptr;
        if(!PyArg_ParseTuple(args, "s", &str)) {
            return Py_None;
        }
        if(str) {
            lele_obj->setText(str);
        }
    }
    return Py_None;
}

PyMemberDef PyLeleLabel::_members[] = {
    PY_LELEOBJECT_MEMBERS()
    PY_LELELABEL_MEMBERS()
    {nullptr}  /* Sentinel */
};

PyMethodDef PyLeleLabel::_methods[] = {
    PY_LELEOBJECT_METHODS()
    PY_LELELABEL_METHODS()
    {nullptr}  /* Sentinel */
};

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
    PyLeleLabel::_methods,             /* tp_methods */
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
