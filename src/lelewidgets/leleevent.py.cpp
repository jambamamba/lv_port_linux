#include <lelewidgets/leleevent.h>
#include <lelewidgets/leleobject.h>
#include <enumobject.h>
#include <debug_logger/debug_logger.h>

LOG_CATEGORY(LVSIM, "LVSIM");

PyObject *LeleEvent::createPyObject() {
    
    // LOG(DEBUG, LVSIM, "createPyObject\n");
    PyTypeObject *type = &PyLeleEvent::_obj_type;
    PyType_Ready(type);
    PyLeleEvent *self = (PyLeleEvent *)type->tp_alloc(type, 0);
    if (!self) {
        return Py_None;
    }
    self->_object = _target_obj ? _target_obj->createPyObject() : Py_None;
    self->_id = PyUnicode_FromString(_id.size() ? _id.c_str() : "");
    if (self->_id == nullptr) {
        Py_DECREF(self);
        return nullptr;
    }
    self->_action = PyUnicode_FromString(_action.size() ? _action.c_str() : "");
    if (self->_action == nullptr) {
        Py_DECREF(self);
        return nullptr;
    }
    self->_args = PyUnicode_FromString("");
    if (self->_args == nullptr) {
        Py_DECREF(self);
        return nullptr;
    }
    self->_type = reinterpret_cast<PyObject*>(&PyLeleEventType::_obj_type);
    if (self->_type == nullptr) {
        Py_DECREF(self);
        return nullptr;
    }
    switch(_code) {
        case LeleEvent::Type::Clicked: self->_code = PyObject_GetAttrString(self->_type, "Clicked"); break;
        case LeleEvent::Type::ValueChanged: self->_code = PyObject_GetAttrString(self->_type, "ValueChanged"); break;
        default: self->_code = Py_None;
    }
    self->_ivalue = _ivalue;
    self->_ivalue2 = _ivalue2;

    return (PyObject *)self;
}

int PyLeleEvent::init(PyLeleEvent *self, PyObject *args, PyObject *kwds) {
    // PyLeleEvent *self = reinterpret_cast<PyLeleEvent *>(self_);
    return 0;
}

void PyLeleEvent::dealloc(PyLeleEvent* self) {
    Py_XDECREF(self->_id);
    Py_XDECREF(self->_object);
    Py_XDECREF(self->_type);
    Py_XDECREF(self->_action);
    Py_XDECREF(self->_args);
    Py_XDECREF(self->_code);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyMemberDef PyLeleEvent::_members[] = {
    PY_LELEEVENT_MEMBERS() \
    {nullptr}  /* Sentinel */
};

PyMethodDef PyLeleEvent::_methods[] = {
    PY_LELEEVENT_METHODS() \
    {nullptr}  /* Sentinel */

};

static PyObject *
PyType_New(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LeleEvent obj;
    return obj.createPyObject();
}
PyTypeObject PyLeleEvent::_obj_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "lele.Event",             /* tp_name */
    sizeof(PyLeleEvent), /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)PyLeleEvent::dealloc, /* tp_dealloc */
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
    Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE,   /* tp_flags */
    "Event object",            /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyLeleEvent::_methods,             /* tp_methods */
    PyLeleEvent::_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyLeleEvent::init,      /* tp_init */
    0,                         /* tp_alloc */
    PyType_New,//PyType_GenericNew,                 /* tp_new */
};

///////////////////////////////////////////////////////////////////////////
int PyLeleEventType::init(PyObject *self_, PyObject *args, PyObject *kwds) {
    PyLeleEventType *self = reinterpret_cast<PyLeleEventType *>(self_);
    self->_clicked = PyLong_FromLong(LeleEvent::Type::Clicked);
    self->_value_changed = PyLong_FromLong(LeleEvent::Type::ValueChanged);
    return 0;
}

void PyLeleEventType::dealloc(PyObject* self_) {
    PyLeleEventType *self = reinterpret_cast<PyLeleEventType *>(self_);
    Py_XDECREF(self->_clicked);
    Py_XDECREF(self->_value_changed);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyMemberDef PyLeleEventType::_members[] = {
    PY_LELEEVENT_TYPE_MEMBERS() \
    {nullptr}  /* Sentinel */
};

//osm todo: google "make PyTypeObject iteratable"
PyTypeObject PyLeleEventType::_obj_type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "lele.Event.Type",                    /* tp_name */
    sizeof(PyLeleEventType),             /* tp_basicsize */
    0,                              /* tp_itemsize */
    /* methods */
    (destructor)PyLeleEventType::dealloc, /* tp_dealloc */
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
    "Event Type",            /* tp_doc */
    0,//(traverseproc)enum_traverse,    /* tp_traverse */
    0,                              /* tp_clear */
    0,                              /* tp_richcompare */
    0,                              /* tp_weaklistoffset */
    PyObject_SelfIter,              /* tp_iter */
    0,//(iternextfunc)enum_next,        /* tp_iternext */
    0,                             /* tp_methods */
    PyLeleEventType::_members,             /* tp_members */
    0,                              /* tp_getset */
    &PyEnum_Type,                              /* tp_base */
    0,                              /* tp_dict */
    0,                              /* tp_descr_get */
    0,                              /* tp_descr_set */
    0,                              /* tp_dictoffset */
    PyLeleEventType::init,                              /* tp_init */
    PyType_GenericAlloc,            /* tp_alloc */
    PyType_GenericNew,                       /* tp_new */
    0,//PyObject_GC_Del,                /* tp_free */
};