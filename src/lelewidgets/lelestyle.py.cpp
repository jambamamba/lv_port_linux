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
    py_obj->_layout = reinterpret_cast<PyObject *>(&PyLeleStyleLayout::_obj_type);
    // py_obj->_layout = LeleObject::createPyEnum("Layout", {
    //         {"Flex",LV_LAYOUT_FLEX},
    //         {"Grid",LV_LAYOUT_GRID},
    //         {"No",LV_LAYOUT_NONE}
    //     }
    // );
    if (py_obj->_layout == nullptr) {
        return false;
    }
    py_obj->_flow = reinterpret_cast<PyObject *>(&PyLeleStyleFlow::_obj_type);
    // py_obj->_flow = LeleObject::createPyEnum("Flow", {
    //         {"Row",LV_FLEX_FLOW_ROW},
    //         {"Column",LV_FLEX_FLOW_COLUMN},
    //         {"RowWrap",LV_FLEX_FLOW_ROW_WRAP},
    //         {"RowReverse",LV_FLEX_FLOW_ROW_REVERSE},
    //         {"RowWrapReverse",LV_FLEX_FLOW_ROW_WRAP_REVERSE},
    //         {"ColumnWrap",LV_FLEX_FLOW_COLUMN_WRAP},
    //         {"ColumnReverse",LV_FLEX_FLOW_COLUMN_REVERSE},
    //         {"ColumnWrapReverse",LV_FLEX_FLOW_COLUMN_WRAP_REVERSE}
    //     }
    // );
    if (py_obj->_flow == nullptr) {
        return false;
    }
    py_obj->_scrollbar = reinterpret_cast<PyObject *>(&PyLeleStyleScrollbar::_obj_type);
    // py_obj->_scrollbar = LeleObject::createPyEnum("Scrollbar", {
    //         {"Off",LV_SCROLLBAR_MODE_OFF},
    //         {"On",LV_SCROLLBAR_MODE_ON},
    //         {"Active",LV_SCROLLBAR_MODE_ACTIVE},
    //         {"Auto",LV_SCROLLBAR_MODE_AUTO}
    //     }
    // );
    if (py_obj->_scrollbar == nullptr) {
        return false;
    }
    py_obj->_border = reinterpret_cast<PyObject *>(&PyLeleStyleBorder::_obj_type);
    // py_obj->_border = LeleObject::createPyEnum("Border", {
    //         {"No",LeleStyle::BorderTypeE::None},
    //         {"Solid",LeleStyle::BorderTypeE::Solid},
    //         {"Dashed",LeleStyle::BorderTypeE::Dashed},
    //         {"Dotted",LeleStyle::BorderTypeE::Dotted}
    //     }
    // );
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

namespace {
PyObject *toPyObject(const PyLeleStyle *py_style, const std::optional<LeleStyle::StyleValue> &style) {

    PyObject *value = Py_None;
    if (std::holds_alternative<int>(style.value())) {
        value = PyLong_FromLong(std::get<int>(style.value()));
    }
    else if (std::holds_alternative<std::string>(style.value())) {
        value = PyUnicode_FromString(std::get<std::string>(style.value()).c_str());
    }
    else if (std::holds_alternative<lv_layout_t>(style.value())) {
        int ival = static_cast<int>(std::get<lv_layout_t>(style.value()));
        switch(ival) {
            case LV_LAYOUT_FLEX: value = PyObject_GetAttrString(py_style->_layout, "Flex"); break;
            case LV_LAYOUT_GRID: value = PyObject_GetAttrString(py_style->_layout, "Grid"); break;
            case LV_LAYOUT_NONE: default: value = PyObject_GetAttrString(py_style->_layout, "No"); break;
        }
    }
    else if (std::holds_alternative<lv_flex_flow_t>(style.value())) {
        int ival = static_cast<int>(std::get<lv_flex_flow_t>(style.value()));
        switch(ival) {
            case LV_FLEX_FLOW_COLUMN: value = PyObject_GetAttrString(py_style->_flow, "Column"); break;
            case LV_FLEX_FLOW_ROW_WRAP: value = PyObject_GetAttrString(py_style->_flow, "RowWrap"); break;
            case LV_FLEX_FLOW_ROW_REVERSE: value = PyObject_GetAttrString(py_style->_flow, "RowReverse"); break;
            case LV_FLEX_FLOW_ROW_WRAP_REVERSE: value = PyObject_GetAttrString(py_style->_flow, "RowWrapReverse"); break;
            case LV_FLEX_FLOW_COLUMN_WRAP: value = PyObject_GetAttrString(py_style->_flow, "ColumnWrap"); break;
            case LV_FLEX_FLOW_COLUMN_REVERSE: value = PyObject_GetAttrString(py_style->_flow, "ColumnReverse"); break;
            case LV_FLEX_FLOW_COLUMN_WRAP_REVERSE: value = PyObject_GetAttrString(py_style->_flow, "ColumnWrapReverse"); break;
            case LV_FLEX_FLOW_ROW: default: value = PyObject_GetAttrString(py_style->_flow, "Row"); break;
        }
    }
    else if (std::holds_alternative<lv_scrollbar_mode_t>(style.value())) {
        int ival = static_cast<int>(std::get<lv_scrollbar_mode_t>(style.value()));
        switch(ival) {
            case LV_SCROLLBAR_MODE_OFF: value = PyObject_GetAttrString(py_style->_scrollbar, "Off"); break;
            case LV_SCROLLBAR_MODE_ON: value = PyObject_GetAttrString(py_style->_scrollbar, "On"); break;
            case LV_SCROLLBAR_MODE_ACTIVE: value = PyObject_GetAttrString(py_style->_scrollbar, "Active"); break;
            case LV_SCROLLBAR_MODE_AUTO: default: value = PyObject_GetAttrString(py_style->_scrollbar, "Auto"); break;
        }
    }
    else if (std::holds_alternative<LeleStyle::BorderTypeE>(style.value())) {
        int ival = static_cast<int>(std::get<LeleStyle::BorderTypeE>(style.value()));
        switch(ival) {
            case LeleStyle::BorderTypeE::Solid: value = PyObject_GetAttrString(py_style->_border, "Solid"); break;
            case LeleStyle::BorderTypeE::Dashed: value = PyObject_GetAttrString(py_style->_border, "Dashed"); break;
            case LeleStyle::BorderTypeE::Dotted: value = PyObject_GetAttrString(py_style->_border, "Dotted"); break;
            case LeleStyle::BorderTypeE::None: default: value = PyObject_GetAttrString(py_style->_border, "No"); break;
        }
    }
    else if (std::holds_alternative<LeleStyle::Rotation>(style.value())) {
        //osm todo
    }
    return value;
}
}//namespace

PyObject *PyLeleStyle::toPyDict(const std::map<std::string, std::optional<LeleStyle::StyleValue>> &&style_name_value_map) {

    struct raii {
        PyObject *_dict;
        std::map<PyObject*, PyObject*> _items;
        ~raii() {            
            for(const auto &[name, value] : _items) {
                Py_XDECREF(name);
                Py_XDECREF(value);
            }
            Py_XDECREF(_dict);
        }
    }$;

    $._dict = PyDict_New();
    if(!$._dict) {
        return PyBool_FromLong(false);
    }
    LeleStyle lele_style("{}");
    PyLeleStyle *py_style = reinterpret_cast<PyLeleStyle *>(lele_style.createPyObject());
    for(const auto &[name, value] : style_name_value_map) {

        if(!value) {
            continue;
        }
        PyObject *py_name = PyUnicode_FromString(name.c_str());
        PyObject *py_value = toPyObject(py_style, value);
        $._items[py_name] = py_value;
        if(PyDict_SetItem($._dict, py_name, py_value) == -1) {
            return PyBool_FromLong(false);
        }
    }
    Py_XDECREF(py_style);
    $._items.clear();
    PyObject *dict = $._dict;
    $._dict = nullptr;
    return dict;
}

PyObject *PyLeleStyle::getValue(PyObject *self_, PyObject *arg) {
    PyLeleStyle *self = reinterpret_cast<PyLeleStyle *>(self_);
    LeleStyle *lele_style = self->_lele_style;
    if(!lele_style) {
        LOG(WARNING, LVSIM, "There are no styles!\n");
        return Py_None;
    }
    //osm todo, if given a list of names, return dict with only these name/value pairs
    return PyLeleStyle::toPyDict(lele_style->getStyle());
}
PyObject *PyLeleStyle::setValue(PyObject *self_, PyObject *arg) {
    PyLeleStyle *self = reinterpret_cast<PyLeleStyle *>(self_);
    return Py_None;//osm todo PyLeleStyle::setValue with dict
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

///////////////////////////////////////////////////////////////////////////
int PyLeleStyleLayout::init(PyObject *self_, PyObject *args, PyObject *kwds) {
    PyLeleStyleLayout *self = reinterpret_cast<PyLeleStyleLayout *>(self_);
    self->_flex = PyLong_FromLong(LeleEvent::Type::Clicked);
    self->_grid = PyLong_FromLong(LeleEvent::Type::Clicked);
    self->_no = PyLong_FromLong(LeleEvent::Type::Clicked);
    return 0;
}

void PyLeleStyleLayout::dealloc(PyObject* self_) {
    PyLeleStyleLayout *self = reinterpret_cast<PyLeleStyleLayout *>(self_);
    Py_XDECREF(self->_flex);
    Py_XDECREF(self->_grid);
    Py_XDECREF(self->_no);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyMemberDef PyLeleStyleLayout::_members[] = {
    PY_LELESTYLE_LAYOUT_MEMBERS() \
    {nullptr}  /* Sentinel */
};

//osm todo: google "make PyTypeObject iteratable"
PyTypeObject PyLeleStyleLayout::_obj_type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "lele.Style.Layout",                    /* tp_name */
    sizeof(PyLeleStyleLayout),             /* tp_basicsize */
    0,                              /* tp_itemsize */
    /* methods */
    (destructor)PyLeleStyleLayout::dealloc, /* tp_dealloc */
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
    "Style Layout Type",            /* tp_doc */
    0,//(traverseproc)enum_traverse,    /* tp_traverse */
    0,                              /* tp_clear */
    0,                              /* tp_richcompare */
    0,                              /* tp_weaklistoffset */
    PyObject_SelfIter,              /* tp_iter */
    0,//(iternextfunc)enum_next,        /* tp_iternext */
    0,                             /* tp_methods */
    PyLeleStyleLayout::_members,             /* tp_members */
    0,                              /* tp_getset */
    &PyEnum_Type,                              /* tp_base */
    0,                              /* tp_dict */
    0,                              /* tp_descr_get */
    0,                              /* tp_descr_set */
    0,                              /* tp_dictoffset */
    PyLeleStyleLayout::init,                              /* tp_init */
    PyType_GenericAlloc,            /* tp_alloc */
    PyType_GenericNew,                       /* tp_new */
    PyObject_GC_Del,                /* tp_free */
};

///////////////////////////////////////////////////////////////////////////
int PyLeleStyleFlow::init(PyObject *self_, PyObject *args, PyObject *kwds) {
    PyLeleStyleFlow *self = reinterpret_cast<PyLeleStyleFlow *>(self_);
    self->_row = PyLong_FromLong(LV_FLEX_FLOW_ROW);
    self->_column = PyLong_FromLong(LV_FLEX_FLOW_COLUMN);
    self->_row_wrap = PyLong_FromLong(LV_FLEX_FLOW_ROW_WRAP);
    self->_row_reverse = PyLong_FromLong(LV_FLEX_FLOW_ROW_REVERSE);
    self->_row_wrap_reverse = PyLong_FromLong(LV_FLEX_FLOW_ROW_WRAP_REVERSE);
    self->_column_wrap = PyLong_FromLong(LV_FLEX_FLOW_COLUMN_WRAP);
    self->_column_reverse = PyLong_FromLong(LV_FLEX_FLOW_COLUMN_REVERSE);
    self->_column_wrap_reverse = PyLong_FromLong(LV_FLEX_FLOW_COLUMN_WRAP_REVERSE);
    return 0;
}

void PyLeleStyleFlow::dealloc(PyObject* self_) {
    PyLeleStyleFlow *self = reinterpret_cast<PyLeleStyleFlow *>(self_);
    Py_XDECREF(self->_row);
    Py_XDECREF(self->_column);
    Py_XDECREF(self->_row_wrap);
    Py_XDECREF(self->_row_reverse);
    Py_XDECREF(self->_row_wrap_reverse);
    Py_XDECREF(self->_column_wrap);
    Py_XDECREF(self->_column_reverse);
    Py_XDECREF(self->_column_wrap_reverse);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyMemberDef PyLeleStyleFlow::_members[] = {
    PY_LELESTYLE_FLOW_MEMBERS() \
    {nullptr}  /* Sentinel */
};

//osm todo: google "make PyTypeObject iteratable"
PyTypeObject PyLeleStyleFlow::_obj_type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "lele.Style.Flow",                    /* tp_name */
    sizeof(PyLeleStyleFlow),             /* tp_basicsize */
    0,                              /* tp_itemsize */
    /* methods */
    (destructor)PyLeleStyleFlow::dealloc, /* tp_dealloc */
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
    "Style Flow Type",            /* tp_doc */
    0,//(traverseproc)enum_traverse,    /* tp_traverse */
    0,                              /* tp_clear */
    0,                              /* tp_richcompare */
    0,                              /* tp_weaklistoffset */
    PyObject_SelfIter,              /* tp_iter */
    0,//(iternextfunc)enum_next,        /* tp_iternext */
    0,                             /* tp_methods */
    PyLeleStyleFlow::_members,             /* tp_members */
    0,                              /* tp_getset */
    &PyEnum_Type,                              /* tp_base */
    0,                              /* tp_dict */
    0,                              /* tp_descr_get */
    0,                              /* tp_descr_set */
    0,                              /* tp_dictoffset */
    PyLeleStyleFlow::init,                              /* tp_init */
    PyType_GenericAlloc,            /* tp_alloc */
    PyType_GenericNew,                       /* tp_new */
    PyObject_GC_Del,                /* tp_free */
};


///////////////////////////////////////////////////////////////////////////
int PyLeleStyleScrollbar::init(PyObject *self_, PyObject *args, PyObject *kwds) {
    PyLeleStyleScrollbar *self = reinterpret_cast<PyLeleStyleScrollbar *>(self_);
    self->_off = PyLong_FromLong(LV_SCROLLBAR_MODE_OFF);
    self->_on = PyLong_FromLong(LV_SCROLLBAR_MODE_ON);
    self->_active = PyLong_FromLong(LV_SCROLLBAR_MODE_ACTIVE);
    self->_auto = PyLong_FromLong(LV_SCROLLBAR_MODE_AUTO);
    return 0;
}

void PyLeleStyleScrollbar::dealloc(PyObject* self_) {
    PyLeleStyleScrollbar *self = reinterpret_cast<PyLeleStyleScrollbar *>(self_);
    Py_XDECREF(self->_off);
    Py_XDECREF(self->_on);
    Py_XDECREF(self->_active);
    Py_XDECREF(self->_auto);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyMemberDef PyLeleStyleScrollbar::_members[] = {
    PY_LELESTYLE_SCROLLBAR_MEMBERS() \
    {nullptr}  /* Sentinel */
};

//osm todo: google "make PyTypeObject iteratable"
PyTypeObject PyLeleStyleScrollbar::_obj_type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "lele.Style.Scrollbar",                    /* tp_name */
    sizeof(PyLeleStyleScrollbar),             /* tp_basicsize */
    0,                              /* tp_itemsize */
    /* methods */
    (destructor)PyLeleStyleScrollbar::dealloc, /* tp_dealloc */
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
    "Style Scrollbar Type",            /* tp_doc */
    0,//(traverseproc)enum_traverse,    /* tp_traverse */
    0,                              /* tp_clear */
    0,                              /* tp_richcompare */
    0,                              /* tp_weaklistoffset */
    PyObject_SelfIter,              /* tp_iter */
    0,//(iternextfunc)enum_next,        /* tp_iternext */
    0,                             /* tp_methods */
    PyLeleStyleScrollbar::_members,             /* tp_members */
    0,                              /* tp_getset */
    &PyEnum_Type,                              /* tp_base */
    0,                              /* tp_dict */
    0,                              /* tp_descr_get */
    0,                              /* tp_descr_set */
    0,                              /* tp_dictoffset */
    PyLeleStyleScrollbar::init,                              /* tp_init */
    PyType_GenericAlloc,            /* tp_alloc */
    PyType_GenericNew,                       /* tp_new */
    PyObject_GC_Del,                /* tp_free */
};


///////////////////////////////////////////////////////////////////////////
int PyLeleStyleBorder::init(PyObject *self_, PyObject *args, PyObject *kwds) {
    PyLeleStyleBorder *self = reinterpret_cast<PyLeleStyleBorder *>(self_);
    self->_no = PyLong_FromLong(LeleStyle::BorderTypeE::None);
    self->_solid = PyLong_FromLong(LeleStyle::BorderTypeE::Solid);
    self->_dashed = PyLong_FromLong(LeleStyle::BorderTypeE::Dashed);
    self->_dotted = PyLong_FromLong(LeleStyle::BorderTypeE::Dotted);
    return 0;
}

void PyLeleStyleBorder::dealloc(PyObject* self_) {
    PyLeleStyleBorder *self = reinterpret_cast<PyLeleStyleBorder *>(self_);
    Py_XDECREF(self->_no);
    Py_XDECREF(self->_solid);
    Py_XDECREF(self->_dashed);
    Py_XDECREF(self->_dotted);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyMemberDef PyLeleStyleBorder::_members[] = {
    PY_LELESTYLE_BORDER_MEMBERS() \
    {nullptr}  /* Sentinel */
};

//osm todo: google "make PyTypeObject iteratable"
PyTypeObject PyLeleStyleBorder::_obj_type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "lele.Style.Border",                    /* tp_name */
    sizeof(PyLeleStyleBorder),             /* tp_basicsize */
    0,                              /* tp_itemsize */
    /* methods */
    (destructor)PyLeleStyleBorder::dealloc, /* tp_dealloc */
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
    "Style Border Type",            /* tp_doc */
    0,//(traverseproc)enum_traverse,    /* tp_traverse */
    0,                              /* tp_clear */
    0,                              /* tp_richcompare */
    0,                              /* tp_weaklistoffset */
    PyObject_SelfIter,              /* tp_iter */
    0,//(iternextfunc)enum_next,        /* tp_iternext */
    0,                             /* tp_methods */
    PyLeleStyleBorder::_members,             /* tp_members */
    0,                              /* tp_getset */
    &PyEnum_Type,                              /* tp_base */
    0,                              /* tp_dict */
    0,                              /* tp_descr_get */
    0,                              /* tp_descr_set */
    0,                              /* tp_dictoffset */
    PyLeleStyleBorder::init,                              /* tp_init */
    PyType_GenericAlloc,            /* tp_alloc */
    PyType_GenericNew,                       /* tp_new */
    PyObject_GC_Del,                /* tp_free */
};