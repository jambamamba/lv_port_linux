#include <lelewidgets/lelemessagebox.h>

LOG_CATEGORY(LVSIM, "LVSIM");

PyObject *LeleMessageBox::createPyObject() const {
    PyTypeObject *type = &PyLeleMessageBox::_obj_type;
    PyType_Ready(type);
    PyLeleMessageBox *self = (PyLeleMessageBox *)type->tp_alloc(type, 0);
    if(!initPyObject(reinterpret_cast<PyLeleObject *>(self))) {
        Py_DECREF(self);
        return nullptr;
    }
    return (PyObject *)self;
}

bool LeleMessageBox::initPyObject(PyLeleObject *py_obj_) const {
    PyLeleMessageBox *py_obj = reinterpret_cast<PyLeleMessageBox *>(py_obj_);
    if(!py_obj) {
        return false;
    }
    return LeleLabel::initPyObject(reinterpret_cast<PyLeleObject *>(&py_obj->ob_base));
}

int PyLeleMessageBox::init(PyObject *self_, PyObject *args, PyObject *kwds) {
    PyLeleObject::fromConfig(self_, args);
    return 0;
}

void PyLeleMessageBox::dealloc(PyObject* self_) {
    PyLeleMessageBox *self = reinterpret_cast<PyLeleMessageBox *>(self_);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject *PyLeleMessageBox::getButtonClicked(PyObject *self_, PyObject *arg) {
    LOG(DEBUG, LVSIM, "PyLeleMessageBox::getButtonClicked\n");
    PyLeleMessageBox *self = reinterpret_cast<PyLeleMessageBox *>(self_);
    LeleMessageBox *lele_obj = dynamic_cast<LeleMessageBox *>(self->ob_base.ob_base._lele_obj);
    if (lele_obj) {
        LeleButtons::LeleButton *lele_btn = lele_obj->getButtonClicked();
        if(lele_btn) {
            return lele_btn->createPyObject();
        }
    }
    return Py_None;
}

PyObject *PyLeleMessageBox::getTitle(PyObject *self_, PyObject *arg) {
    PyLeleMessageBox *self = reinterpret_cast<PyLeleMessageBox *>(self_);
    LeleMessageBox *lele_obj = dynamic_cast<LeleMessageBox *>(self->ob_base.ob_base._lele_obj);
    if (lele_obj) {
        return PyUnicode_FromString(
            lele_obj->getTitle().size() ? 
                lele_obj->getTitle().c_str() : 
                "");
    }
    return Py_None;
}

PyObject *PyLeleMessageBox::setTitle(PyObject *self_, PyObject *args) {
    PyLeleMessageBox *self = reinterpret_cast<PyLeleMessageBox *>(self_);
    LeleMessageBox *lele_obj = dynamic_cast<LeleMessageBox *>(self->ob_base.ob_base._lele_obj);
    if(lele_obj && args) {
        char *value = nullptr;
        if(!PyArg_ParseTuple(args, "s", &value)) {
            return Py_None;
        }
        lele_obj->setTitle(value);
    }
    return Py_None;
}

PyMemberDef PyLeleMessageBox::_members[] = {
    PY_LELEMESSAGEBOX_MEMBERS()
    {nullptr}  /* Sentinel */
};

PyMethodDef PyLeleMessageBox::_methods[] = {
    PY_LELEMESSAGEBOX_METHODS()
    {nullptr}  /* Sentinel */
};

PyTypeObject PyLeleMessageBox::_obj_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "lele.MessageBox",             /* tp_name */
    sizeof(PyLeleMessageBox), /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)PyLeleMessageBox::dealloc, /* tp_dealloc */
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
    PyLeleMessageBox::_methods,             /* tp_methods */
    PyLeleMessageBox::_members,             /* tp_members */
    0,                         /* tp_getset */
    &PyLeleLabel::_obj_type,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyLeleMessageBox::init,      /* tp_init */
    0,                         /* tp_alloc */
    PyType_GenericNew,                 /* tp_new */
};
