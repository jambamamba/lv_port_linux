#include <lelewidgets/lelerollerview.h>

PyObject *LeleRollerView::createPyObject() {
    PyTypeObject *type = &PyLeleRollerView::_obj_type;
    PyType_Ready(type);
    PyLeleRollerView *self = (PyLeleRollerView *)type->tp_alloc(type, 0);
    if(!initPyObject(reinterpret_cast<PyLeleObject *>(self))) {
        Py_DECREF(self);
        return nullptr;
    }
    return (PyObject *)self;
}

bool LeleRollerView::initPyObject(PyLeleObject *py_obj_) {
    PyLeleRollerView *py_obj = reinterpret_cast<PyLeleRollerView *>(py_obj_);
    if(!py_obj) {
        return false;
    }
    return LeleObject::initPyObject(reinterpret_cast<PyLeleObject *>(&py_obj->ob_base));
}

int PyLeleRollerView::init(PyObject *self_, PyObject *args, PyObject *kwds) {
    PyLeleObject::fromConfig(self_, args);
    return 0;
}

void PyLeleRollerView::dealloc(PyObject* self_) {
    PyLeleRollerView *self = reinterpret_cast<PyLeleRollerView *>(self_);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject *PyLeleRollerView::getText(PyObject *self_, PyObject *arg) {
    PyLeleRollerView *self = reinterpret_cast<PyLeleRollerView *>(self_);
    LeleRollerView *lele_obj = dynamic_cast<LeleRollerView *>(self->ob_base._lele_obj);
    if(lele_obj) {
        return PyUnicode_FromString(lele_obj->getText().c_str());
    }
    return Py_None;
}

PyObject *PyLeleRollerView::setText(PyObject *self_, PyObject *args) {
    PyLeleRollerView *self = reinterpret_cast<PyLeleRollerView *>(self_);
    LeleRollerView *lele_obj = dynamic_cast<LeleRollerView *>(self->ob_base._lele_obj);
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

PyMemberDef PyLeleRollerView::_members[] = {
    PY_LELEROLLERVIEW_MEMBERS()
    {nullptr}  /* Sentinel */
};

PyMethodDef PyLeleRollerView::_methods[] = {
    PY_LELEROLLERVIEW_METHODS()
    {nullptr}  /* Sentinel */
};

PyTypeObject PyLeleRollerView::_obj_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "lele.Label",             /* tp_name */
    sizeof(PyLeleRollerView), /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)PyLeleRollerView::dealloc, /* tp_dealloc */
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
    PyLeleRollerView::_methods,             /* tp_methods */
    PyLeleRollerView::_members,             /* tp_members */
    0,                         /* tp_getset */
    &PyLeleObject::_obj_type,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyLeleRollerView::init,      /* tp_init */
    0,                         /* tp_alloc */
    PyType_GenericNew,                 /* tp_new */
};
