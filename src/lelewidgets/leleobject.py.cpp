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

    // LOG(DEBUG, LVSIM, "LeleObject::pyCallback:'%p'\n", py_callback);
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
        PyObject *ret=nullptr;
        ~RAII() {
            Py_XDECREF(global_dict);
            Py_XDECREF(local_dict);
            Py_XDECREF(ret);            
        }
    };
}//namespace

// PyObject* LeleObject::getPyEnumValue(const std::string &enum_value) {
//     std::string code = 
//     "import lele\n"
//     "res=";
//     code += enum_value;
//     RAII $;
//     $.global_dict = PyDict_New();
//     if (!$.global_dict) { return Py_None; }
//     $.local_dict = PyDict_New();
//     if (!$.local_dict) { return Py_None; }
//     $.ret = PyRun_String(code.c_str(), Py_file_input, $.global_dict, $.local_dict);

//     if (!$.ret) { 
//         PyErr_Print();
//         LOG(FATAL, LVSIM, "Failed in PyRun_String!\n'%s'\n", code.c_str());
//         return Py_None; 
//     }
//     PyObject *output = PyDict_GetItemString($.local_dict, "res");
//     if (!output) {
//         // PyDict_GetItemString does not set exceptions
//         LOG(WARNING, LVSIM, "Failed in PyDict_GetItemString!\n");
//         PyErr_SetString(PyExc_KeyError, "could not get 'res'");
//         return Py_None;
//     } else {
//         Py_INCREF(output);
//     }
//     return output;
// }

// PyObject* LeleObject::createPyEnum(const std::string &enum_name, const std::map<std::string,int> &&enum_map) {//https://stackoverflow.com/a/69290003
//     std::string enum_str =
//     "from enum import Enum\n"
//     "class ";
//     enum_str += enum_name;
//     enum_str += "(Enum):\n";

//     for(auto &[key,value] : enum_map) {
//         enum_str += "    " + key + " = " + std::to_string(value) + "\n";
//     }
//     enum_str += "";
//     RAII $;
//     $.global_dict = PyDict_New();
//     if (!$.global_dict) { 
//         return Py_None; 
//     }
//     $.ret = PyRun_String(enum_str.c_str(), Py_file_input, $.global_dict, $.local_dict);
//     if (!$.ret) { 
//         PyErr_Print();
//         LOG(FATAL, LVSIM, "Failed in PyRun_String!\n'%s'\n", enum_str.c_str());
//         return Py_None; 
//     }
//     PyObject *output = PyDict_GetItemString($.global_dict, enum_name.c_str());
//     if (!output) {
//         // PyDict_GetItemString does not set exceptions
//         PyErr_SetString(PyExc_KeyError, "could not get enum_name");
//         return Py_None;
//     } else {
//         Py_INCREF(output);
//     }
//     return output;
// }

PyObject *LeleObject::createPyEnum(const std::string &enum_name, const std::map<std::string,int> &&enum_map) {

    // Source - https://stackoverflow.com/a
    // Posted by Cyrille Pontvieux
    // Retrieved 2025-12-22, License - CC BY-SA 4.0
    
    const char *k1;
    PyObject *attrs = PyDict_New();
    for(const auto &[name,value] : enum_map) {
        k1 = name.c_str();
        PyObject *key = PyUnicode_FromString(name.c_str());
        PyObject *val = PyLong_FromLong(value);
        PyObject_SetItem(attrs, key, val);
        Py_DECREF(key);
        Py_DECREF(val);
    }

    PyObject *name = PyUnicode_FromString(enum_name.c_str());
    PyObject *args = PyTuple_Pack(2, name, attrs);//same as Py_BuildValue("(OO)", name, attrs);//
    Py_DECREF(attrs);
    Py_DECREF(name);

    // the module name might need to be passed as keyword argument
    PyObject *kwargs = PyDict_New();
    PyObject *key = PyUnicode_FromString("module");
    PyObject *modname = PyModule_GetNameObject(getPyModule());
    PyObject_SetItem(kwargs, key, modname);
    Py_DECREF(key);
    Py_DECREF(modname);

    PyObject *enum_mod = getEnumModule();//PyImport_ImportModule("enum");
    PyObject *enum_type = PyObject_GetAttrString(enum_mod, "Enum");
    PyObject *sub_enum_type = PyObject_Call(enum_type, args, kwargs);//same as calling 'enum.Enum('FooBar', dict(FOO=1, BAR=2))'
    Py_DECREF(enum_type);
    Py_DECREF(args);
    Py_DECREF(kwargs);

    return sub_enum_type;
}

int PyLeleObject::init(PyObject *self_, PyObject *args, PyObject *kwds) {
    PyLeleObject::fromConfig(self_, args);
    return 0;
}

void PyLeleObject::dealloc(PyObject* self_) {
    PyLeleObject *self = reinterpret_cast<PyLeleObject *>(self_);
    Py_XDECREF(self->_id);
    Py_XDECREF(self->_class_name);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject *PyLeleObject::pyListOrPyObjectFromStdVector(const std::vector<PyObject*> &py_objects) {
    if(py_objects.size() == 0) {
        return Py_None;
    }
    else if(py_objects.size() == 1) {
        Py_INCREF(py_objects.at(0));
        return py_objects.at(0);
    }
    else {//(py_objects.size() > 1) 
        PyObject *list = PyList_New(0);
        for(PyObject *py_object : py_objects){
            PyList_Append(list, py_object);
        }
        Py_INCREF(list);
        return list;
    }
}

PyObject *PyLeleObject::addChild(PyObject *self_, PyObject *args) {
    PyLeleObject *self = reinterpret_cast<PyLeleObject *>(self_);
    const char* config = nullptr;
    if(!PyArg_ParseTuple(args, "s", //str
                &config)) {
        LOG(WARNING, LVSIM, "Failed to parse args\n");
        return Py_None;
    }
    if(!config) {
        LOG(WARNING, LVSIM, "Could not get config file\n");
        return Py_None;
    }
    LeleObject *lele_obj = dynamic_cast<LeleObject *>(self->_lele_obj);
    if(!lele_obj) {
        LOG(WARNING, LVSIM, "This object (the parent) is not initialized and cannot be used to add a child object. Was it loaded from a config file?\n");
        return Py_None;
    }
    auto nodes = LeleWidgetFactory::fromConfig(lele_obj, config);
    std::vector<PyObject*> py_objects;
    for (const auto &[key,token]: nodes) {
        if (std::holds_alternative<std::unique_ptr<LeleObject>>(token)) {
            LeleObject *lele_obj = std::get<std::unique_ptr<LeleObject>>(token).get();
            PyObject *py_obj = lele_obj->createPyObject();
            if(!py_obj){
                LOG(WARNING, LVSIM, "Could not create PyObject!\n");
                return Py_None;
            }
            py_objects.emplace_back(py_obj);
        }
    }
    if(py_objects.size() > 0) {
        lele_obj->children().insert(
            lele_obj->children().end(), 
            std::make_move_iterator(nodes.begin()), std::make_move_iterator(nodes.end()));
    }
    return pyListOrPyObjectFromStdVector(py_objects);
}

PyObject *PyLeleObject::fromConfig(PyObject *self_, PyObject *args) {
    PyLeleObject *self = reinterpret_cast<PyLeleObject *>(self_);
    PyObject *parent = nullptr;
    const char* config = nullptr;
    if(!PyArg_ParseTuple(args, "Os", //parent obj, str
                &parent,
                &config)) {
        LOG(WARNING, LVSIM, "Failed to parse args\n");
        return Py_None;
    }
    if(!parent) {
        LOG(WARNING, LVSIM, "Could not get parent\n");
        return Py_None;
    }
    if(!config) {
        LOG(WARNING, LVSIM, "Could not get config file\n");
        return Py_None;
    }
    PyLeleObject *py_parent = reinterpret_cast<PyLeleObject *>(parent);
    LeleObject *lele_parent = dynamic_cast<LeleObject *>(py_parent->_lele_obj);
    if(!lele_parent) {
        LOG(WARNING, LVSIM, "Could not get parent\n");
        return Py_None;
    }
    auto nodes = LeleWidgetFactory::fromConfig(lele_parent, config);
    for (const auto &[key,token]: nodes) {
        if (std::holds_alternative<std::unique_ptr<LeleObject>>(token)) {
            self->_lele_obj = std::get<std::unique_ptr<LeleObject>>(token).get();
            if(!self->_lele_obj->initPyObject(self)) {
                Py_DECREF(self);
                return nullptr;
            }
            break;//osm todo: ensure the json config has just one object, becasue we only instantiate one object
        }
    }
    self->_lele_obj->children().insert(
        self->_lele_obj->children().end(), 
        std::make_move_iterator(nodes.begin()), std::make_move_iterator(nodes.end()));
    return reinterpret_cast<PyObject *>(self_);
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
    if(!lele_obj || !args) {
        return PyBool_FromLong(false);
    }
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


PyObject *PyLeleObject::getStyle(PyObject *self_, PyObject *args) {
    PyLeleObject *self = reinterpret_cast<PyLeleObject *>(self_);
    LeleObject *lele_obj = dynamic_cast<LeleObject *>(self->_lele_obj);
    if(!lele_obj || !args) {
        return PyBool_FromLong(false);
    }

    //osm todo: split into getStyleById, and getStyleAttributes
    //osm todo: Style, add methods: getValue->dict, setValue(dict)

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
    for(const auto &[name, style] : lele_obj->getStyle()) {

        if(!style) {
            continue;
        }
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
        PyObject *name_ = PyUnicode_FromString(name.c_str());
        $._items[name_] = value;
        if(PyDict_SetItem($._dict, name_, value) == -1) {
            return PyBool_FromLong(false);
        }
    }
    Py_XDECREF(py_style);
    $._items.clear();
    PyObject *dict = $._dict;
    $._dict = nullptr;
    return dict;
}

PyObject *PyLeleObject::addStyle(PyObject *self_, PyObject *args) {
    PyLeleObject *self = reinterpret_cast<PyLeleObject *>(self_);
    LeleObject *lele_obj = dynamic_cast<LeleObject *>(self->_lele_obj);
    if(!lele_obj || !args) {
        return PyBool_FromLong(false);
    }
    Py_ssize_t num_args = PyTuple_Size(args);
    if(num_args != 1) {
        return PyBool_FromLong(false);
    }
    PyLeleStyle *py_style = nullptr;
    if(!PyArg_ParseTuple(args, "O", //obj
        &py_style)) {
        return PyBool_FromLong(false);
    }
    Py_XINCREF(py_style);
    lele_obj->addStyle(py_style->_lele_styles);
    return PyBool_FromLong(true);
}

PyObject *PyLeleObject::removeStyle(PyObject *self_, PyObject *args) {
    PyLeleObject *self = reinterpret_cast<PyLeleObject *>(self_);
    LeleObject *lele_obj = dynamic_cast<LeleObject *>(self->_lele_obj);
    if(lele_obj && args) {
        //osm todo
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

static PyObject *
PyType_New(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LeleObject obj;
    return obj.createPyObject();
}

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
    PyType_New,                 /* tp_new */
};
