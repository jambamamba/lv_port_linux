#include <lelewidgets/leleimage.h>

PyObject *LeleImage::createPyObject() {
    PyTypeObject *type = &PyLeleImage::_obj_type;
    PyType_Ready(type);
    PyLeleImage *self = (PyLeleImage *)type->tp_alloc(type, 0);
    if(!initPyObject(reinterpret_cast<PyLeleObject *>(self))) {
        Py_DECREF(self);
        return nullptr;
    }
    return (PyObject *)self;
}

bool LeleImage::initPyObject(PyLeleObject *py_obj_) {
    PyLeleImage *py_obj = reinterpret_cast<PyLeleImage *>(py_obj_);
    if(!py_obj) {
        return false;
    }
    return LeleObject::initPyObject(reinterpret_cast<PyLeleObject *>(&py_obj->ob_base));
}

int PyLeleImage::init(PyObject *self_, PyObject *args, PyObject *kwds) {
    PyLeleObject::fromConfig(self_, args);
    return 0;
}

void PyLeleImage::dealloc(PyObject* self_) {
    PyLeleImage *self = reinterpret_cast<PyLeleImage *>(self_);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject *PyLeleImage::getBuffer(PyObject *self_, PyObject *arg) {
    PyLeleImage *self = reinterpret_cast<PyLeleImage *>(self_);
    LeleImage *lele_obj = dynamic_cast<LeleImage *>(self->ob_base._lele_obj);
    if(lele_obj) {
        const auto &buffer = lele_obj->getBuffer();
        // PyBytes_FromStringAndSize(const char *str, Py_ssize_t size)
        return PyBytes_FromStringAndSize(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    }
    return Py_None;
}

PyObject *PyLeleImage::setBuffer(PyObject *self_, PyObject *args) {
    PyLeleImage *self = reinterpret_cast<PyLeleImage *>(self_);
    LeleImage *lele_obj = dynamic_cast<LeleImage *>(self->ob_base._lele_obj);
    if(lele_obj && args) {
        char *bytes = nullptr;
        Py_ssize_t size = 0;
        if(!PyArg_ParseTuple(args, "y#", &bytes, &size)) {
            // y# (read-only bytes-like object) [const char *, Py_ssize_t]
            // w* (read-write bytes-like object) [Py_buffer],  must call PyBuffer_Release() when done 
            // s# (read-only bytes-like object, also accepts Unicode) [const char *, Py_ssize_t],  string may contain embedded null bytes, as the length is provide separately
            return Py_None;
        }
        if(bytes && size > 0) {
            std::vector<uint8_t> buffer(bytes, bytes + size);
            lele_obj->setBuffer(buffer);
        }
    }
    return Py_None;
}

PyObject *PyLeleImage::getSrc(PyObject *self_, PyObject *arg) {
    PyLeleImage *self = reinterpret_cast<PyLeleImage *>(self_);
    LeleImage *lele_obj = dynamic_cast<LeleImage *>(self->ob_base._lele_obj);
    if(lele_obj) {
        return PyUnicode_FromString(lele_obj->getSrc().c_str());
    }
    return Py_None;
}

PyObject *PyLeleImage::setSrc(PyObject *self_, PyObject *args) {
    PyLeleImage *self = reinterpret_cast<PyLeleImage *>(self_);
    LeleImage *lele_obj = dynamic_cast<LeleImage *>(self->ob_base._lele_obj);
    if(lele_obj && args) {
        char *str = nullptr;
        if(!PyArg_ParseTuple(args, "s", &str)) {
            return Py_None;
        }
        if(str) {
            lele_obj->setSrc(str);
        }
    }
    return Py_None;
}

PyObject *PyLeleImage::getSize(PyObject *self_, PyObject *arg) {
    PyLeleImage *self = reinterpret_cast<PyLeleImage *>(self_);
    LeleImage *lele_obj = dynamic_cast<LeleImage *>(self->ob_base._lele_obj);
    if(lele_obj) {
        auto [x, y] = lele_obj->getSize();
        PyObject *xy = PyTuple_New(2);
        PyTuple_SetItem(xy, 0, PyLong_FromLong(x));
        PyTuple_SetItem(xy, 1, PyLong_FromLong(y));
        return xy;
    }
    return Py_None;
}
PyObject *PyLeleImage::setSize(PyObject *self_, PyObject *args) {
    PyLeleImage *self = reinterpret_cast<PyLeleImage *>(self_);
    LeleImage *lele_obj = dynamic_cast<LeleImage *>(self->ob_base._lele_obj);
    if(lele_obj && args) {
        int x = 0;
        int y = 0;
        if(!PyArg_ParseTuple(args, "ii", &x, &y)) {
            return Py_False;
        }
        lele_obj->setSize(x, y);
    }
    return Py_True;
}
PyObject *PyLeleImage::getPosition(PyObject *self_, PyObject *arg) {
    PyLeleImage *self = reinterpret_cast<PyLeleImage *>(self_);
    LeleImage *lele_obj = dynamic_cast<LeleImage *>(self->ob_base._lele_obj);
    if(lele_obj) {
        auto [x, y] = lele_obj->getPosition();
        PyObject *xy = PyTuple_New(2);
        PyTuple_SetItem(xy, 0, PyLong_FromLong(x));
        PyTuple_SetItem(xy, 1, PyLong_FromLong(y));
        return xy;
    }
    return Py_None;
}
PyObject *PyLeleImage::setPosition(PyObject *self_, PyObject *args) {
    PyLeleImage *self = reinterpret_cast<PyLeleImage *>(self_);
    LeleImage *lele_obj = dynamic_cast<LeleImage *>(self->ob_base._lele_obj);
    if(lele_obj && args) {
        int x = 0;
        int y = 0;
        if(!PyArg_ParseTuple(args, "ii", &x, &y)) {
            return Py_False;
        }
        lele_obj->setPosition(x, y);
    }
    return Py_True;
}
PyObject *PyLeleImage::getRotation(PyObject *self_, PyObject *arg) {
    PyLeleImage *self = reinterpret_cast<PyLeleImage *>(self_);
    LeleImage *lele_obj = dynamic_cast<LeleImage *>(self->ob_base._lele_obj);
    if(lele_obj) {
        auto [angle, pivot_x, pivot_y] = lele_obj->getRotation();
        PyObject *ret = PyTuple_New(3);
        PyTuple_SetItem(ret, 0, PyLong_FromLong(angle));
        PyTuple_SetItem(ret, 1, PyLong_FromLong(pivot_x));
        PyTuple_SetItem(ret, 2, PyLong_FromLong(pivot_y));
        return ret;
    }
    return Py_None;
}
PyObject *PyLeleImage::setRotation(PyObject *self_, PyObject *args) {
    PyLeleImage *self = reinterpret_cast<PyLeleImage *>(self_);
    LeleImage *lele_obj = dynamic_cast<LeleImage *>(self->ob_base._lele_obj);
    if(lele_obj && args) {
        float angle = 0;
        int x = 0;
        int y = 0;
        if(!PyArg_ParseTuple(args, "fii", &angle, &x, &y)) {
            return Py_None;
        }
        lele_obj->setRotation(angle, x, y);
    }
    return Py_None;
}

PyMemberDef PyLeleImage::_members[] = {
    PY_LELEIMAGE_MEMBERS()
    {nullptr}  /* Sentinel */
};

PyMethodDef PyLeleImage::_methods[] = {
    PY_LELEIMAGE_METHODS()
    {nullptr}  /* Sentinel */
};

PyTypeObject PyLeleImage::_obj_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "lele.Image",             /* tp_name */
    sizeof(PyLeleImage), /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)PyLeleImage::dealloc, /* tp_dealloc */
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
    PyLeleImage::_methods,             /* tp_methods */
    PyLeleImage::_members,             /* tp_members */
    0,                         /* tp_getset */
    &PyLeleObject::_obj_type,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyLeleImage::init,      /* tp_init */
    0,                         /* tp_alloc */
    PyType_GenericNew,                 /* tp_new */
};
