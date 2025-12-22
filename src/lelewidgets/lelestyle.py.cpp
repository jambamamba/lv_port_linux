#include <lelewidgets/leleobject.h>
#include <lelewidgets/lelestyle.h>
#include <lelewidgets/lelewidgetfactory.h>

LOG_CATEGORY(LVSIM, "LVSIM");

PyObject *LeleStyle::createPyObject() {
    PyTypeObject *type = &PyLeleStyle::_obj_type;
    PyType_Ready(type);
    PyLeleStyle *self = (PyLeleStyle *)type->tp_alloc(type, 0);
    if(!initPyObject(self)) {
        Py_DECREF(self);
        return nullptr;
    }
    return (PyObject *)self;
}

bool LeleStyle::initPyObject(PyLeleStyle *py_obj) {
    if(!py_obj) {
        return false;
    }
    py_obj->_lele_style = this;
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
    py_obj->_layout = LeleObject::createPyEnum("Layout", {
            {"Flex",LV_LAYOUT_FLEX},
            {"Grid",LV_LAYOUT_GRID},
            {"No",LV_LAYOUT_NONE}
        }
    );
    if (py_obj->_layout == nullptr) {
        return false;
    }
    py_obj->_flow = LeleObject::createPyEnum("Flow", {
            {"Row",LV_FLEX_FLOW_ROW},
            {"Column",LV_FLEX_FLOW_COLUMN},
            {"RowWrap",LV_FLEX_FLOW_ROW_WRAP},
            {"RowReverse",LV_FLEX_FLOW_ROW_REVERSE},
            {"RowWrapReverse",LV_FLEX_FLOW_ROW_WRAP_REVERSE},
            {"ColumnWrap",LV_FLEX_FLOW_COLUMN_WRAP},
            {"ColumnReverse",LV_FLEX_FLOW_COLUMN_REVERSE},
            {"ColumnWrapReverse",LV_FLEX_FLOW_COLUMN_WRAP_REVERSE}
        }
    );
    if (py_obj->_flow == nullptr) {
        return false;
    }
    py_obj->_scrollbar = LeleObject::createPyEnum("Scrollbar", {
            {"Off",LV_SCROLLBAR_MODE_OFF},
            {"On",LV_SCROLLBAR_MODE_ON},
            {"Active",LV_SCROLLBAR_MODE_ACTIVE},
            {"Auto",LV_SCROLLBAR_MODE_AUTO}
        }
    );
    if (py_obj->_scrollbar == nullptr) {
        return false;
    }
    py_obj->_border = LeleObject::createPyEnum("Border", {
            {"No",LeleStyle::BorderTypeE::None},
            {"Solid",LeleStyle::BorderTypeE::Solid},
            {"Dashed",LeleStyle::BorderTypeE::Dashed},
            {"Dotted",LeleStyle::BorderTypeE::Dotted}
        }
    );
    if (py_obj->_border == nullptr) {
        return false;
    }
    return true;
}

int PyLeleStyle::init(PyObject *self_, PyObject *args, PyObject *kwds) {
    PyLeleStyle::fromConfig(self_, args);
    return 0;
}

void PyLeleStyle::dealloc(PyObject* self_) {
    PyLeleStyle *self = reinterpret_cast<PyLeleStyle *>(self_);
    Py_XDECREF(self->_id);
    Py_XDECREF(self->_class_name);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject *PyLeleStyle::fromConfig(PyObject *self_, PyObject *args) {
    PyLeleStyle *self = reinterpret_cast<PyLeleStyle *>(self_);
    Py_ssize_t num_args = PyTuple_Size(args);
    const char* config = nullptr;
    if(num_args == 0) {
        const char empty[] = "{}";
        config = empty;
    }
    else {
        if(!PyArg_ParseTuple(args, "s", //str
                    &config)) {
            LOG(WARNING, LVSIM, "Failed to parse args\n");
            return Py_None;
        }
        if(!config) {
            LOG(WARNING, LVSIM, "Could not get config file\n");
            return Py_None;
        }
    }
    self->_lele_styles = LeleWidgetFactory::stylesFromConfig(config);
    if(self->_lele_styles.size() > 0) {
        self->_lele_style = self->_lele_styles.at(0).get();
    }
    else {
        LOG(WARNING, LVSIM, "There are no styles!\n");
        self->_lele_style = nullptr;
    }
    return reinterpret_cast<PyObject*>(self);
}

PyObject *PyLeleStyle::getClassName(PyObject *self_, PyObject *arg) {
    PyLeleStyle *self = reinterpret_cast<PyLeleStyle *>(self_);
    if (!self->_class_name) {
        PyErr_SetString(PyExc_AttributeError, "class_name");
        return Py_None;
    }
    return self->_class_name;
}

PyMemberDef PyLeleStyle::_members[] = {
    PY_LELESTYLE_MEMBERS()
    {nullptr}  /* Sentinel */
};

PyMethodDef PyLeleStyle::_methods[] = {
    PY_LELESTYLE_METHODS()
    {nullptr}  /* Sentinel */
};

static PyObject *
PyType_New(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LeleStyle obj;
    return obj.createPyObject();
}

PyTypeObject PyLeleStyle::_obj_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "lele.Style",             /* tp_name */
    sizeof(PyLeleStyle), /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)PyLeleStyle::dealloc, /* tp_dealloc */
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
    "Style Object",            /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    PyLeleStyle::_methods,             /* tp_methods */
    PyLeleStyle::_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyLeleStyle::init,      /* tp_init */
    0,                         /* tp_alloc */
    PyType_New,                 /* tp_new */
};
