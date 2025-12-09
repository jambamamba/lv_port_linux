#include <filesystem>
#include <iostream>
#include <debug_logger/debug_logger.h>

#include "python_wrapper.h"
#include "graphics_backend.h"
#include "lelewidgets/leleobject.h"

LOG_CATEGORY(LVSIM, "LVSIM");

namespace {
    static GraphicsBackend _graphics_backend;
    static std::vector<std::pair<std::string, LeleWidgetFactory::Node>> _nodes;
    // static PyObject* createPyModule();

    // static PyObject* _mymodule_exec(PyObject* spec) {
    //     LOG(FATAL, LVSIM, "_mymodule_exec\n");
    //     return nullptr;
    // }
    // static PyObject* _mymodule_create(PyObject* spec) {
    //     PyObject *module = createPyModule();
    //     return module;
    // }
    //Functions that py script can call
    static PyObject* _mymodule_version(PyObject *self, PyObject *args) {
        int major_version = 1;//(int)(This->GetVersion().toFloat());
        int minor_version = 0;//(int)((This->GetVersion().toFloat() - major_version) * 100.);
        PyObject *dict = PyDict_New();
        PyDict_SetItem(dict, PyUnicode_FromString("Major"), PyLong_FromDouble(major_version));
        PyDict_SetItem(dict, PyUnicode_FromString("Minor"), PyLong_FromDouble(minor_version));
            return dict;
    }
    static PyObject* _mymodule_addEventHandler(PyObject *self, PyObject *args) {
        char *id = nullptr;
        PyObject *callback = nullptr;
        if(!PyArg_ParseTuple(args, "sO", //id, obj
            &id,
            &callback)) {
            return PyLong_FromLong(0);
        }
        // LOG(DEBUG, LVSIM, "@@@> _mymodule_addEventHandler id:'%s'\n", id);
        LeleWidgetFactory::iterateNodes(_nodes, 0, [id, callback](LeleObject &lele_object) {
            if(lele_object.id() == id) {
                Py_XINCREF(callback);
                lele_object.addEventHandler(callback);
            }
        });
        return PyLong_FromLong(1);
    }
    static PyObject* _mymodule_getObjectById(PyObject *self, PyObject *args) {
        char *id = nullptr;
        if(!PyArg_ParseTuple(args, "s", //id
            &id)) {
            return PyLong_FromLong(0);
        }
        std::vector<PyObject*> py_objects;
        LeleWidgetFactory::iterateNodes(_nodes, 0, [id,&py_objects](LeleObject &lele_object) {
            if(lele_object.id() == id) {
                PyObject *py_obj = lele_object.createPyObject();
                if(!py_obj){
                    LOG(FATAL, LVSIM, "Could not create PyObject!\n");
                    return;
                }
                py_objects.emplace_back(py_obj);
            }
        });
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
    static PyObject* _mymodule_foo(PyObject *self, PyObject *args) {
        int num = 0;
        char *str = nullptr;
        PyObject *list = nullptr;
        PyObject *dic = nullptr;
        PyObject *callback = nullptr;
        if(!PyArg_ParseTuple(args, "isOOO", //int,str,obj,obj,obj
            &num,
            &str,
            &list,
            &dic,
            &callback)) {
            return PyLong_FromLong(0);
        }
        std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "num: " << num << "\n";
        std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "str: " << str << "\n";

        int len = PyList_Size(list);
        while (len--) {
            std::string str(PyUnicode_AsUTF8(PyList_GetItem(list, len)));
            std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "List item: " << str << "\n";
        }
        // Py_DECREF(list);

        PyObject* keys = PyDict_Keys(dic); 
        Py_ssize_t size = PyList_Size(keys);
        for (Py_ssize_t i = 0; i < size; i++) {
            PyObject* key = PyList_GetItem(keys, i);
            PyObject* key_str = PyObject_Str(key);
            const char* key_c_str = PyUnicode_AsUTF8(key_str);
            PyObject *value_str = PyDict_GetItemString(dic, key_c_str);
            const char* value_c_str = PyUnicode_AsUTF8(value_str);
            std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "Dic key: " << key_c_str << ", value: " << value_c_str << "\n";
            Py_DECREF(key_str);
            Py_DECREF(value_str);
        }
        // Py_DECREF(dic);     

        PyObject *arglist = Py_BuildValue("(s)", "hello from c++");
        PyObject *res = PyObject_CallObject(callback, arglist);
        if(res) { Py_DECREF(res); }
        // Py_DECREF(callback);

        return PyLong_FromLong(1);
    }
    static PyObject* _mymodule_loadConfig(PyObject *self, PyObject *args) {
        char *str = nullptr;
        if(!PyArg_ParseTuple(args, "s", //str
            &str)) {
            LOG(WARNING, LVSIM, "Failed to parse config name\n");
            return PyBool_FromLong(false);
        }
        if(!str) {
            LOG(WARNING, LVSIM, "Failed to parse config name\n");
            return PyBool_FromLong(false);
        }
        LOG(DEBUG, LVSIM, "Load config '%s'\n", str);

        std::string dir = LeleObject::getPyScriptDir();
        LOG(FATAL, LVSIM, "SCRIPT dir '%s'\n", dir.c_str());

        std::string input_file(str);
        if(input_file.size() > 2 && str[0] == '.' && str[1] == '/') {
            input_file = std::filesystem::current_path().string() + "/" + input_file.substr(2);
        }

        if(!std::filesystem::exists(input_file)) {
            LOG(WARNING, LVSIM, "Failed to load config, file not found: '%s'\n", input_file.c_str());
            return PyBool_FromLong(false);
        }
        _nodes = LeleWidgetFactory::fromConfig(input_file);
        if(_nodes.size() == 0) {
            LOG(WARNING, LVSIM, "Failed to load config: '%s'\n", input_file.c_str());
            return PyBool_FromLong(false);
        }
        return PyBool_FromLong(true);
    }
    static PyObject* _mymodule_handleEvents(PyObject *self, PyObject *args) {
        if(!_graphics_backend.handleEvents()) {
            return PyLong_FromLong(0);
        }
        return PyLong_FromLong(1);
    }
    static PyMethodDef _mymodule_methods[] = {
        {"version", _mymodule_version, METH_VARARGS, "lele.version()"},
        {"foo", _mymodule_foo, METH_VARARGS, "lele.foo(num, str, list, dic, callback)"},
        {"loadConfig", _mymodule_loadConfig, METH_VARARGS, "lele.loadConfig(/path/to/config/json)"},
        {"handleEvents", _mymodule_handleEvents, METH_VARARGS, "lele.handleEvents()"},
        {"addEventHandler", _mymodule_addEventHandler, METH_VARARGS, "lele.addEventHandler(callback)"},
        {"getObjectById", _mymodule_getObjectById, METH_VARARGS, "lele.getObjectById(id)"},
        {nullptr, nullptr, 0, nullptr} // Sentinel
    };
    static PyModuleDef_Slot _mymodule_slots[] = {
        // {Py_mod_create, (void*)_mymodule_create},
        // {Py_mod_exec, _mymodule_exec},
        {0, nullptr} // Sentinel to mark the end of the array
    };    
    static PyModuleDef _mymodule = {
        PyModuleDef_HEAD_INIT, //m_base
        "lele", //m_name
        nullptr, //m_doc
        0, //m_size // m_size must be non-negative (0) for multi-phase init, and can be -1 
        _mymodule_methods, //m_methods
        nullptr,// _mymodule_slots, //m_slots //PyModule_Create is incompatible with m_slots
        nullptr, //m_traverse
        nullptr, //m_clear
        nullptr //m_free
    };
    // static PyObject* createPyModule() {
    //     // PyObject *module_spec = PyObject_CallFunctionObjArgs((PyObject *)&PyModuleSpec_Type, nullptr);

    //     // PyObject *module_spec = PyObject_GetAttr(mod, &_Py_ID(__spec__));
    //     PyObject *name = PyUnicode_FromString("lele");
    //     PyObject *attrs = Py_BuildValue("{sO}", "name", name);
    //     PyObject *module_spec = _PyNamespace_New(attrs);

    //     PyObject* module_name = PyUnicode_FromString("lele");
    //     PyObject_SetAttrString(module_spec, "name", module_name);
    //     Py_DECREF(module_name); // Decrement reference count for the name object

    //     PyObject* origin = PyObject_GetAttrString(module_spec, "origin");
    //     PyObject_SetAttrString(module_spec, "__file__", origin);
    //     Py_DECREF(origin);

    //     PyObject *module = PyModule_FromDefAndSpec(&_mymodule, module_spec);
    //     PyModule_ExecDef(module, &_mymodule);

    //     // PyModule_AddObject(mod, "foo", PyUnicode_FromString("bar"));//will show up in Python as lele.foo with value "bar"
    //     return module;
    // }
    
}//namespace

/////////////////////////////////////////////////////////////////////
PyMODINIT_FUNC PyInit_lele(void) {
    if(!_graphics_backend.load()) {
        LOG(FATAL, LVSIM, "Failed to load graphcis backend\n");
        return nullptr;
    }
   //Either  multi-phase initialization:
    // return PyModuleDef_Init(&_mymodule);//leads to _mymodule_create
    //Or single stage:
    return PyModule_Create(&_mymodule);
}