#include <lelewidgets/lelebutton.h>
#include <enumobject.h>
#include <debug_logger/debug_logger.h>

LOG_CATEGORY(LVSIM, "LVSIM");


PyObject *LeleButtons::LeleButton::createPyObject() {
    PyTypeObject *type = &PyLeleButton::_obj_type;
    PyType_Ready(type);
    PyLeleButton *self = (PyLeleButton *)type->tp_alloc(type, 0);
    if(!initPyObject(reinterpret_cast<PyLeleObject *>(self))) {
        Py_DECREF(self);
        return nullptr;
    }
    return (PyObject *)self;
}

bool LeleButtons::LeleButton::initPyObject(PyLeleObject *py_obj_) {
    PyLeleButton *py_obj = reinterpret_cast<PyLeleButton *>(py_obj_);
    if(!py_obj) {
        return false;
    }
    // py_obj->_type = LeleObject::createPyEnum("Type", {
    //         {"Push",LeleButton::Type::Push},
    //         {"Checkbox",LeleButton::Type::Checkbox},
    //         {"Radio",LeleButton::Type::Radio},
    //         {"Switch",LeleButton::Type::Switch},
    //         {"Close",LeleButton::Type::Close},
    //         {"Slider",LeleButton::Type::Slider}
    //     }
    // );
    py_obj->_type = reinterpret_cast<PyObject*>(&PyLeleButtonType::_obj_type);
    if (py_obj->_type == nullptr) {
        return false;
    }
    return LeleLabel::initPyObject(reinterpret_cast<PyLeleObject *>(&py_obj->ob_base));
}

int PyLeleButton::init(PyObject *self_, PyObject *args, PyObject *kwds) {
    PyLeleObject::fromConfig(self_, args);
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

PyObject *PyLeleButton::getChecked(PyObject *self_, PyObject *arg) {
    PyLeleButton *self = reinterpret_cast<PyLeleButton *>(self_);
    LeleButtons::LeleButton *lele_obj = dynamic_cast<LeleButtons::LeleButton *>(self->ob_base.ob_base._lele_obj);
    if (lele_obj) {
        return PyBool_FromLong(lele_obj->getChecked());
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
        std::string enum_type = "";
        switch(lele_obj->getType()) {
            case LeleButtons::LeleButton::Type::Push: return PyObject_GetAttrString(self->_type, "Push");
            case LeleButtons::LeleButton::Type::Checkbox: return PyObject_GetAttrString(self->_type, "Checkbox");
            case LeleButtons::LeleButton::Type::Radio: return PyObject_GetAttrString(self->_type, "Radio");
            case LeleButtons::LeleButton::Type::Switch: return PyObject_GetAttrString(self->_type, "Switch");
            case LeleButtons::LeleButton::Type::Close: return PyObject_GetAttrString(self->_type, "Close");
            case LeleButtons::LeleButton::Type::Slider: return PyObject_GetAttrString(self->_type, "Slider");
            default:break;
        }
    }
    return Py_None;
}

PyObject *PyLeleButton::click(PyObject *self_, PyObject *arg) {
    PyLeleButton *self = reinterpret_cast<PyLeleButton *>(self_);
    LeleButtons::LeleButton *lele_obj = dynamic_cast<LeleButtons::LeleButton *>(self->ob_base.ob_base._lele_obj);
    if (lele_obj) {
        PyBool_FromLong(false);
    }
    return PyBool_FromLong(lele_obj->click());
}

PyObject *PyLeleButton::getValue(PyObject *self_, PyObject *arg) {
    PyLeleButton *self = reinterpret_cast<PyLeleButton *>(self_);
    LeleButtons::LeleButton *lele_obj = dynamic_cast<LeleButtons::LeleButton *>(self->ob_base.ob_base._lele_obj);
    if (lele_obj) {
        return PyLong_FromLong(lele_obj->getValue());
    }
    return PyBool_FromLong(false);
}

PyObject *PyLeleButton::setValue(PyObject *self_, PyObject *args) {
    PyLeleButton *self = reinterpret_cast<PyLeleButton *>(self_);
    LeleButtons::LeleButton *lele_obj = dynamic_cast<LeleButtons::LeleButton *>(self->ob_base.ob_base._lele_obj);
    if(lele_obj && args) {
        long value = 0;
        if(!PyArg_ParseTuple(args, "i", &value)) {
            return Py_None;
        }
        lele_obj->setValue(value);
    }
    return Py_None;
}

PyMemberDef PyLeleButton::_members[] = {
    PY_LELEBUTTON_MEMBERS()
    {nullptr}  /* Sentinel */
};

PyMethodDef PyLeleButton::_methods[] = {
    PY_LELEBUTTON_METHODS()
    {nullptr}  /* Sentinel */
};

static PyObject *
PyType_New(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LeleButtons::LeleButton obj;
    return obj.createPyObject();
}

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
    &PyLeleLabel::_obj_type,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyLeleButton::init,      /* tp_init */
    0,                         /* tp_alloc */
    PyType_New,                 /* tp_new */
};
///////////////////////////////////////////////////////////////////////////
int PyLeleButtonType::init(PyObject *self_, PyObject *args, PyObject *kwds) {
    PyLeleButtonType *self = reinterpret_cast<PyLeleButtonType *>(self_);

    self->_push = PyLong_FromLong(LeleButtons::LeleButton::Type::Push);
    self->_checkbox = PyLong_FromLong(LeleButtons::LeleButton::Type::Checkbox);
    self->_radio = PyLong_FromLong(LeleButtons::LeleButton::Type::Radio);
    self->_switch = PyLong_FromLong(LeleButtons::LeleButton::Type::Switch);
    self->_close = PyLong_FromLong(LeleButtons::LeleButton::Type::Close);
    self->_slider = PyLong_FromLong(LeleButtons::LeleButton::Type::Slider);
    return 0;
}

void PyLeleButtonType::dealloc(PyObject* self_) {
    PyLeleButtonType *self = reinterpret_cast<PyLeleButtonType *>(self_);
    Py_XDECREF(self->_push);
    Py_XDECREF(self->_checkbox);
    Py_XDECREF(self->_radio);
    Py_XDECREF(self->_switch);
    Py_XDECREF(self->_close);
    Py_XDECREF(self->_slider    );
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject *PyLeleButtonType::create(PyTypeObject *type_, PyObject *args, PyObject *kwds) {
    PyTypeObject *type = &PyLeleButtonType::_obj_type;
    PyType_Ready(type);
    PyObject *self = type->tp_alloc(type, 0);
    if(!init(self, args, kwds)) {
        Py_DECREF(self);
        return nullptr;
    }
    return (PyObject *)self;
}

int PyLeleButtonType::enum_traverse(PyObject *self, visitproc visit, void *arg) {
    return (*PyEnum_Type.tp_traverse)(self, visit, arg);
}

PyObject *PyLeleButtonType::enum_next(PyObject *self) {
    return (*PyEnum_Type.tp_iternext)(self);
}

PyMemberDef PyLeleButtonType::_members[] = {
    PY_LELEBUTTON_TYPE_MEMBERS() \
    {nullptr}  /* Sentinel */
};

PyTypeObject PyLeleButtonType::_obj_type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "lele.Button.Type",                    /* tp_name */
    sizeof(PyLeleButtonType),             /* tp_basicsize */
    0,                              /* tp_itemsize */
    /* methods */
    (destructor)PyLeleButtonType::dealloc, /* tp_dealloc */
    0,                              /* tp_vectorcall_offset */
    0,                              /* tp_getattr */
    0,                              /* tp_setattr */
    0,                              /* tp_as_async */
    0,                              /* tp_repr */
    0,                              /* tp_as_number */
    0,                              /* tp_as_sequence */
    0,                              /* tp_as_mapping */
    0,                              /* tp_hash */
    0,                              /* tp_call */
    0,                              /* tp_str */
    PyObject_GenericGetAttr,        /* tp_getattro */
    0,                              /* tp_setattro */
    0,                              /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
    "Button Type",            /* tp_doc */
    (traverseproc)&PyLeleButtonType::enum_traverse,    /* tp_traverse */
    0,                              /* tp_clear */
    0,                              /* tp_richcompare */
    0,                              /* tp_weaklistoffset */
    PyObject_SelfIter,              /* tp_iter */
    (iternextfunc)enum_next,        /* tp_iternext */
    0,                             /* tp_methods */
    PyLeleButtonType::_members,             /* tp_members */
    0,                              /* tp_getset */
    &PyEnum_Type,                              /* tp_base */
    0,                              /* tp_dict */
    0,                              /* tp_descr_get */
    0,                              /* tp_descr_set */
    0,                              /* tp_dictoffset */
    PyLeleButtonType::init,                              /* tp_init */
    PyType_GenericAlloc,            /* tp_alloc */
    PyLeleButtonType::create,                       /* tp_new */
    PyObject_GC_Del,                /* tp_free */
};