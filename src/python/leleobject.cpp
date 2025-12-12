#include <lelewidgets/leleobject.h>

LOG_CATEGORY(LVSIM, "LVSIM");

PyObject *LeleObject::createPyObject() {
    PyTypeObject *type = &PyLeleObject::_obj_type;
    PyType_Ready(type);
    PyLeleObject *self = (PyLeleObject *)type->tp_alloc(type, 0);
    if(!initPyObject(self)) {
        Py_DECREF(self);
        return nullptr;
    }
    return (PyObject *)self;
}

bool LeleObject::initPyObject(PyLeleObject *py_obj) {
    if(!py_obj) {
        return false;
    }
    py_obj->_lele_obj = this;
    py_obj->_id = PyUnicode_FromString(
        _id.size() ? _id.c_str() : "");
    if (py_obj->_id == nullptr) {
        return false;
    }
    py_obj->_class_name = PyUnicode_FromString(
        _class_name.size() ? _class_name.c_str() : "");
    if (py_obj->_class_name == nullptr) {
        return false;
    }
    return true;
}

bool LeleObject::pyCallback(PyObject *py_callback, LeleEvent &&e) {

    LOG(DEBUG, LVSIM, "LeleObject::pyCallback:'%p'\n", py_callback);
    lv_event_t* lv_event = const_cast<lv_event_t*>(e.getLvEvent());
    lv_event_code_t code = lv_event_get_code(lv_event);
    bool ret = false;

    PyObject *py_event = Py_BuildValue("(O)", e.createPyObject());
    if(!py_event) {
        LOG(WARNING, LVSIM, "LeleObject::pyCallback: Py_BuildValue FAILED to create event PyObject\n");
        return ret;
    }
    PyObject *res = PyObject_CallObject(py_callback, py_event);
    if(res) { 
        if(res == Py_None) {
            LOG(DEBUG, LVSIM, "LeleObject::pyCallback returned nothing\n");
            ret = true;
        }
        else {
            int iret = PyObject_IsTrue(res);
            LOG(DEBUG, LVSIM, "LeleObject::pyCallback returned iret:%i\n", iret);
            if(iret == 1) {
                ret = true;
            }//else //We got Py_False or Error
        }
        Py_DECREF(res); 
    }
    return ret;
}

namespace {
    struct RAII {
        PyObject *global_dict=nullptr;
        PyObject *local_dict=nullptr;
        PyObject *should_be_none=nullptr;
        ~RAII() {
            Py_XDECREF(global_dict);
            Py_XDECREF(local_dict);
            Py_XDECREF(should_be_none);            
        }
    };
}//namespace

PyObject* LeleObject::getPyEnumValue(const std::string &enum_value) {
    std::string code = 
    "import lele\n"
    "res=";
    code += enum_value;
    RAII $;
    $.global_dict = PyDict_New();
    if (!$.global_dict) { return Py_None; }
    $.local_dict = PyDict_New();
    if (!$.local_dict) { return Py_None; }
    $.should_be_none = PyRun_String(code.c_str(), Py_file_input, $.global_dict, $.local_dict);

    if (!$.should_be_none) { 
        LOG(WARNING, LVSIM, "Failed in PyRun_String! '%s'\n", code.c_str());
        return Py_None; 
    }
    PyObject *output = PyDict_GetItemString($.local_dict, "res");
    if (!output) {
        // PyDict_GetItemString does not set exceptions
        LOG(WARNING, LVSIM, "Failed in PyDict_GetItemString!\n");
        PyErr_SetString(PyExc_KeyError, "could not get 'res'");
        return Py_None;
    } else {
        Py_INCREF(output);
    }
    return output;
}

PyObject* LeleObject::createPyEnum(const std::string &enum_name, const std::map<std::string,int> &&enum_map) {//https://stackoverflow.com/a/69290003
    std::string enum_str =
    "from enum import Enum\n"
    "class ";
    enum_str += enum_name;
    enum_str += "(Enum):\n";

    for(auto &[key,value] : enum_map) {
        enum_str += "    " + key + " = " + std::to_string(value) + "\n";
    }
    enum_str += "";
    RAII $;
    $.global_dict = PyDict_New();
    if (!$.global_dict) { 
        return Py_None; 
    }
    $.should_be_none = PyRun_String(enum_str.c_str(), Py_file_input, $.global_dict, $.local_dict);
    if (!$.should_be_none) { 
        return Py_None; 
    }
    PyObject *output = PyDict_GetItemString($.global_dict, enum_name.c_str());
    if (!output) {
        // PyDict_GetItemString does not set exceptions
        PyErr_SetString(PyExc_KeyError, "could not get enum_name");
        return Py_None;
    } else {
        Py_INCREF(output);
    }
    return output;
}

int PyLeleObject::init(PyObject *self_, PyObject *args, PyObject *kwds) {
    PyLeleObject *self = reinterpret_cast<PyLeleObject *>(self_);
    // self->_id = PyUnicode_FromString("id");
    // self->_class_name = PyUnicode_FromString("class_name");
    return 0;
}

void PyLeleObject::dealloc(PyObject* self_) {
    PyLeleObject *self = reinterpret_cast<PyLeleObject *>(self_);
    Py_XDECREF(self->_id);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject *PyLeleObject::addChild(PyObject *self_, PyObject *arg) {
    PyLeleObject *self = reinterpret_cast<PyLeleObject *>(self_);
    const char* config = PyUnicode_AsUTF8(arg);
    if(!config) {
        LOG(WARNING, LVSIM, "Could not get config file\n");
        return Py_False;
    }
    LeleObject *lele_obj = dynamic_cast<LeleObject *>(self->_lele_obj);
    if(!lele_obj) {
        LOG(FATAL, LVSIM, "There is no parent object.\n");
        return Py_False;
    }
    auto nodes = LeleWidgetFactory::fromConfig(lele_obj, config);
    self->_child_nodes.insert(
        self->_child_nodes.end(), 
        std::make_move_iterator(nodes.begin()), std::make_move_iterator(nodes.end()));
    return Py_True;
}

PyObject *PyLeleObject::getClassName(PyObject *self_, PyObject *arg) {
    PyLeleObject *self = reinterpret_cast<PyLeleObject *>(self_);
    if (!self->_class_name) {
        PyErr_SetString(PyExc_AttributeError, "class_name");
        return Py_None;
    }
    return self->_class_name;
}

PyObject *PyLeleObject::addEventHandler(PyObject *self_, PyObject *args) {
    PyLeleObject *self = reinterpret_cast<PyLeleObject *>(self_);
    LeleObject *lele_obj = dynamic_cast<LeleObject *>(self->_lele_obj);
    if(lele_obj && args) {
        PyObject *py_callback = nullptr;
        if(!PyArg_ParseTuple(args, "O", //obj
            &py_callback)) {
            return PyBool_FromLong(false);
        }
        Py_XINCREF(py_callback);
        LOG(DEBUG, LVSIM, "PyLeleObject::addEventHandler:'%p'\n", py_callback);
        lele_obj->addEventHandler(py_callback);
        return PyBool_FromLong(true);
    }
    return Py_None;
}

PyMemberDef PyLeleObject::_members[] = {
    PY_LELEOBJECT_MEMBERS()
    {nullptr}  /* Sentinel */
};

PyMethodDef PyLeleObject::_methods[] = {
    PY_LELEOBJECT_METHODS()
    {nullptr}  /* Sentinel */
};

PyTypeObject PyLeleObject::_obj_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "lele.Object",             /* tp_name */
    sizeof(PyLeleObject), /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)PyLeleObject::dealloc, /* tp_dealloc */
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
    PyLeleObject::_methods,             /* tp_methods */
    PyLeleObject::_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyLeleObject::init,      /* tp_init */
    0,                         /* tp_alloc */
    PyType_GenericNew,                 /* tp_new */
};
