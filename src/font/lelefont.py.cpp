#include "lelefont.h"


PyObject *LeleFont::createPyObject() {
    
    // LOG(DEBUG, LVSIM, "createPyObject\n");
    PyTypeObject *type = &PyLeleFont::_obj_type;
    PyType_Ready(type);
    PyLeleFont *self = (PyLeleFont *)type->tp_alloc(type, 0);
    if (!self) {
        return Py_None;
    }
    self->_lele_font = nullptr;
    return (PyObject *)self;
}

int PyLeleFont::init(PyObject *self_, PyObject *args, PyObject *kwds) {
    PyLeleFont *self = reinterpret_cast<PyLeleFont *>(self_);
    return 0;
}

void PyLeleFont::dealloc(PyObject* self) {
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject *PyLeleFont::getFontDb(PyObject *self_, PyObject *args) {
    PyLeleFont *self = reinterpret_cast<PyLeleFont *>(self_);
    LeleFont *lele_font = dynamic_cast<LeleFont *>(self->_lele_font);
    if(self) {
        // self-getFontDb();//osm todo
    }
    return Py_None;
}

PyMemberDef PyLeleFont::_members[] = {
    PY_LELEFONT_MEMBERS()
    {nullptr}  /* Sentinel */
};

PyMethodDef PyLeleFont::_methods[] = {
    PY_LELEFONT_METHODS()
    {nullptr}  /* Sentinel */
};

static PyObject *
PyType_New(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LeleFont obj;
    return obj.createPyObject();
}

PyTypeObject PyLeleFont::_obj_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "lele.Font",             /* tp_name */
    sizeof(PyLeleFont), /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)PyLeleFont::dealloc, /* tp_dealloc */
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
    "Font",            /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyLeleFont::_methods,             /* tp_methods */
    PyLeleFont::_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyLeleFont::init,      /* tp_init */
    0,                         /* tp_alloc */
    PyType_New,                 /* tp_new */
};
