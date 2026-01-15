#include <lelewidgets/lelecolorwheel.h>

LOG_CATEGORY(LVSIM, "LVSIM");

PyObject *LeleColorWheel::createPyObject() {
    PyTypeObject *type = &PyLeleColorWheel::_obj_type;
    PyType_Ready(type);
    PyLeleColorWheel *self = (PyLeleColorWheel *)type->tp_alloc(type, 0);
    if(!initPyObject(reinterpret_cast<PyLeleObject *>(self))) {
        Py_DECREF(self);
        return nullptr;
    }
    return (PyObject *)self;
}

bool LeleColorWheel::initPyObject(PyLeleObject *py_obj_) {
    PyLeleColorWheel *py_obj = reinterpret_cast<PyLeleColorWheel *>(py_obj_);
    if(!py_obj) {
        return false;
    }
    return LeleObject::initPyObject(reinterpret_cast<PyLeleObject *>(&py_obj->ob_base));
}

int PyLeleColorWheel::init(PyObject *self_, PyObject *args, PyObject *kwds) {
    PyLeleObject::fromConfig(self_, args);
    return 0;
}

void PyLeleColorWheel::dealloc(PyObject* self_) {
    PyLeleColorWheel *self = reinterpret_cast<PyLeleColorWheel *>(self_);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject *PyLeleColorWheel::getColor(PyObject *self_, PyObject *args){
    PyLeleColorWheel *self = reinterpret_cast<PyLeleColorWheel *>(self_);
    LeleColorWheel *lele_obj = dynamic_cast<LeleColorWheel *>(self->ob_base._lele_obj);
    if(lele_obj) {
        return PyLong_FromLong(lele_obj->getColor());
    }
    return Py_None;
}

PyObject *PyLeleColorWheel::setColor(PyObject *self_, PyObject *args){
    PyLeleColorWheel *self = reinterpret_cast<PyLeleColorWheel *>(self_);
    LeleColorWheel *lele_obj = dynamic_cast<LeleColorWheel *>(self->ob_base._lele_obj);
    if(lele_obj && args) {
        int value = 0;
        if(!PyArg_ParseTuple(args, "i", &value)) {
            return Py_None;
        }
        if(value) {
            lele_obj->setColor(value);
        }
    }
    return Py_None;
}

PyObject *PyLeleColorWheel::getBgColor(PyObject *self_, PyObject *args){
    PyLeleColorWheel *self = reinterpret_cast<PyLeleColorWheel *>(self_);
    LeleColorWheel *lele_obj = dynamic_cast<LeleColorWheel *>(self->ob_base._lele_obj);
    if(lele_obj) {
        return PyLong_FromLong(lele_obj->getBgColor());
    }
    return Py_None;
}

PyObject *PyLeleColorWheel::setBgColor(PyObject *self_, PyObject *args){
    PyLeleColorWheel *self = reinterpret_cast<PyLeleColorWheel *>(self_);
    LeleColorWheel *lele_obj = dynamic_cast<LeleColorWheel *>(self->ob_base._lele_obj);
    if(lele_obj && args) {
        int value = 0;
        if(!PyArg_ParseTuple(args, "i", &value)) {
            return Py_None;
        }
        if(value) {
            lele_obj->setBgColor(value);
        }
    }
    return Py_None;
}

PyObject *PyLeleColorWheel::onColorChanged(PyObject *self_, PyObject *args) {
    PyLeleColorWheel *self = reinterpret_cast<PyLeleColorWheel *>(self_);
    LeleColorWheel *lele_obj = dynamic_cast<LeleColorWheel *>(self->ob_base._lele_obj);
    if(!lele_obj || !args) {
        return PyBool_FromLong(false);
    }
    PyObject *py_callback = nullptr;
    if(!PyArg_ParseTuple(args, "O", //obj
        &py_callback)) {
        return PyBool_FromLong(false);
    }
    Py_XINCREF(py_callback);
    LOG(DEBUG, LVSIM, "PyLeleColorWheel::onColorChanged:'%p'\n", py_callback);
    lele_obj->onColorChanged(py_callback);
    return PyBool_FromLong(true);
}

PyMemberDef PyLeleColorWheel::_members[] = {
    PY_LELECOLORWHEEL_MEMBERS()
    {nullptr}  /* Sentinel */
};

PyMethodDef PyLeleColorWheel::_methods[] = {
    PY_LELECOLORWHEEL_METHODS()
    {nullptr}  /* Sentinel */
};

PyTypeObject PyLeleColorWheel::_obj_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "lele.ColorWheel",             /* tp_name */
    sizeof(PyLeleColorWheel), /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)PyLeleColorWheel::dealloc, /* tp_dealloc */
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
    PyLeleColorWheel::_methods,             /* tp_methods */
    PyLeleColorWheel::_members,             /* tp_members */
    0,                         /* tp_getset */
    &PyLeleObject::_obj_type,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyLeleColorWheel::init,      /* tp_init */
    0,                         /* tp_alloc */
    PyType_GenericNew,                 /* tp_new */
};
