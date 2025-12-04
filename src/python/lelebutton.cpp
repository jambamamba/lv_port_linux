#include <lelewidgets/lelebutton.h>

PyObject *LeleButtons::LeleButton::createPyObject() {
    PyTypeObject *type = &PyLeleButton::_obj_type;
    PyType_Ready(type);
    PyLeleButton *self = (PyLeleButton *)type->tp_alloc(type, 0);
    if (self != nullptr) {
        self->ob_base.ob_base._lele_obj = this;
        self->_type = createPyEnum({
                {"Push",LeleButton::Type::Push},
                {"Checkbox",LeleButton::Type::Checkbox},
                {"Radio",LeleButton::Type::Radio},
                {"Switch",LeleButton::Type::Switch},
                {"Close",LeleButton::Type::Close},
                {"Slider",LeleButton::Type::Slider}
            }
        );
        if (self->_type == nullptr) {
            Py_DECREF(self);
            return nullptr;
        }
    }
    return (PyObject *)self;
}

int PyLeleButton::init(PyObject *self_, PyObject *args, PyObject *kwds) {
    // PyLeleButton *self = reinterpret_cast<PyLeleButton *>(self_);
    return 0;
}

void PyLeleButton::dealloc(PyObject* self_) {
    PyLeleButton *self = reinterpret_cast<PyLeleButton *>(self_);
    Py_XDECREF(self->_type);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject *PyLeleButton::isCheckable(PyObject *self_, PyObject *arg) {
    PyLeleButton *self = reinterpret_cast<PyLeleButton *>(self_);
    LeleButtons::LeleButton *lele_obj = dynamic_cast<LeleButtons::LeleButton *>(self->ob_base.ob_base._lele_obj);
    if (lele_obj) {
        return PyBool_FromLong(lele_obj->isCheckable());
    }
    return PyBool_FromLong(false);
}

PyObject *PyLeleButton::isChecked(PyObject *self_, PyObject *arg) {
    PyLeleButton *self = reinterpret_cast<PyLeleButton *>(self_);
    LeleButtons::LeleButton *lele_obj = dynamic_cast<LeleButtons::LeleButton *>(self->ob_base.ob_base._lele_obj);
    if (lele_obj) {
        return PyBool_FromLong(lele_obj->isChecked());
    }
    return PyBool_FromLong(false);
}

PyObject *PyLeleButton::setChecked(PyObject *self_, PyObject *args) {
    PyLeleButton *self = reinterpret_cast<PyLeleButton *>(self_);
    LeleButtons::LeleButton *lele_obj = dynamic_cast<LeleButtons::LeleButton *>(self->ob_base.ob_base._lele_obj);
    if(lele_obj && args) {
        long value = 0;
        if(!PyArg_ParseTuple(args, "i", &value)) {
            return Py_None;
        }
        lele_obj->setChecked(value);
    }
    return Py_None;
}

PyObject *PyLeleButton::getType(PyObject *self_, PyObject *arg) {
    PyLeleButton *self = reinterpret_cast<PyLeleButton *>(self_);
    LeleButtons::LeleButton *lele_obj = dynamic_cast<LeleButtons::LeleButton *>(self->ob_base.ob_base._lele_obj);
    if (lele_obj) {
        return PyLong_FromLong(lele_obj->getType());//osm todo: should return const string
    }
    return PyLong_FromLong(0);
}

PyMemberDef PyLeleButton::_members[] = {
    PY_LELEBUTTON_MEMBERS()
    {nullptr}  /* Sentinel */
};

PyMethodDef PyLeleButton::_methods[] = {
    PY_LELEBUTTON_METHODS()
    {nullptr}  /* Sentinel */
};

PyTypeObject PyLeleButton::_obj_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "lele.Button",             /* tp_name */
    sizeof(PyLeleButton), /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)PyLeleButton::dealloc, /* tp_dealloc */
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
    PyLeleButton::_methods,             /* tp_methods */
    PyLeleButton::_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyLeleButton::init,      /* tp_init */
    0,                         /* tp_alloc */
    PyType_GenericNew,                 /* tp_new */
};
