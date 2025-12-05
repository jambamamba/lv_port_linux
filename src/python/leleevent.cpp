#include <lelewidgets/leleevent.h>
#include <lelewidgets/leleobject.h>

PyObject *LeleEvent::createPyObject() {
    PyTypeObject *type = &PyLeleEvent::_obj_type;
    PyType_Ready(type);
    PyLeleEvent *self = (PyLeleEvent *)type->tp_alloc(type, 0);
    if (self != nullptr) {
        self->_object = _target_obj->createPyObject();
        // self->_object_id = PyUnicode_FromString(target_obj_id.size() ? target_obj_id.c_str() : "");
        // if (self->_object_id == nullptr) {
        //     Py_DECREF(self);
        //     return nullptr;
        // }
        self->_event_id = PyUnicode_FromString(_id.size() ? _id.c_str() : "");
        if (self->_event_id == nullptr) {
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
        self->_type = _target_obj->createPyEnum("Type", {
                {"Clicked",LeleEvent::Type::Clicked},
                {"ValueChanged",LeleEvent::Type::ValueChanged}
            }
        );
        self->_code = _code;
        self->_value = _ivalue;
    }
    return (PyObject *)self;
}

int PyLeleEvent::init(PyLeleEvent *self, PyObject *args, PyObject *kwds) {
    self->_event_id = PyUnicode_FromString("event_id");
    self->_object = Py_None;
    self->_type = PyUnicode_FromString("type");
    self->_action = PyUnicode_FromString("action");
    self->_args = PyUnicode_FromString("args");
    self->_code = 0;
    self->_value = 0;
    return 0;
}

void PyLeleEvent::dealloc(PyLeleEvent* self) {
    Py_XDECREF(self->_event_id);
    Py_XDECREF(self->_object);
    Py_XDECREF(self->_type);
    Py_XDECREF(self->_action);
    Py_XDECREF(self->_args);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyMemberDef PyLeleEvent::_members[] = {
    {"event_id", Py_T_OBJECT_EX, offsetof(PyLeleEvent, _event_id), 0, "event_id"},
    {"object", Py_T_OBJECT_EX, offsetof(PyLeleEvent, _object), 0, "object"},
    {"type", Py_T_OBJECT_EX, offsetof(PyLeleEvent, _type), 0, "type"},
    {"action", Py_T_OBJECT_EX, offsetof(PyLeleEvent, _action), 0, "action"},
    {"args", Py_T_OBJECT_EX, offsetof(PyLeleEvent, _args), 0, "args"},
    {"code", Py_T_INT, offsetof(PyLeleEvent, _code), 0, "code"},
    {"value", Py_T_INT, offsetof(PyLeleEvent, _value), 0, "value"},
    {nullptr}  /* Sentinel */
};

PyMethodDef PyLeleEvent::_methods[] = {
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

// static PyMethodDef PyLeleEvent_methods[] = {
//     {"id", (PyCFunction)PyLeleEvent_id, METH_NOARGS, "Return the id"},
//     {"type", (PyCFunction)PyLeleEvent_id, METH_NOARGS, "Return the type"},
//     {"action", (PyCFunction)PyLeleEvent_id, METH_NOARGS, "Return the action"},
//     {"args", (PyCFunction)PyLeleEvent_id, METH_NOARGS, "Return the args"},
//     {nullptr}  /* Sentinel */
// };

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
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /* tp_flags */
    "Event object",            /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    0,//PyLeleEvent_methods,             /* tp_methods */
    PyLeleEvent::_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyLeleEvent::init,      /* tp_init */
    0,                         /* tp_alloc */
    PyType_GenericNew,                 /* tp_new */
};
