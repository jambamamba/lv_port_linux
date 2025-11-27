#include <filesystem>
#include <iostream>
#include <debug_logger/debug_logger.h>

#include "python_wrapper.h"
#include "graphics_backend.h"
#include "lelewidgets/lelebase.h"

LOG_CATEGORY(LVSIM, "LVSIM");

namespace {
    static GraphicsBackend _graphics_backend;
    static std::vector<std::pair<std::string, LeleWidgetFactory::Token>> _tokens;

    //Functions that py script can call
    static PyObject* _mymodule_version(PyObject *self, PyObject *args) {
        int major_version = 1;//(int)(This->GetVersion().toFloat());
        int minor_version = 0;//(int)((This->GetVersion().toFloat() - major_version) * 100.);
        PyObject *dict = PyDict_New();
        PyDict_SetItem(dict, PyUnicode_FromString("Major"), PyLong_FromDouble(major_version));
        PyDict_SetItem(dict, PyUnicode_FromString("Minor"), PyLong_FromDouble(minor_version));
        return dict;
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
            return PyLong_FromLong(-1);
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

        return PyLong_FromLong(0);
    }
    static PyObject* _mymodule_loadConfig(PyObject *self, PyObject *args) {
        char *str = nullptr;
        if(!PyArg_ParseTuple(args, "s", //str
            &str)) {
            return PyLong_FromLong(-1);
        }
        std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "str: " << str << "\n";

        if(!str) {
            return PyLong_FromLong(-1);
        }

        std::string input_file(str);
        if(str[0] != '/') {
            input_file = std::filesystem::current_path().string() + "/" + input_file;
        }
        if((std::filesystem::exists(input_file))) {
            _tokens = LeleWidgetFactory::fromConfig(input_file);
            if(_tokens.size() == 0) {
                return PyLong_FromLong(-1);
            }
        }
        return PyLong_FromLong(0);
    }
    static PyObject* _mymodule_handleEvents(PyObject *self, PyObject *args) {
        PyObject* dict = PyDict_New();
        if(!_graphics_backend.handleEvents()) {
            return dict;//PyLong_FromLong(0);
        }
        PyObject* val1 = PyUnicode_FromString("data_01");
        if (!val1) {
            Py_XDECREF(dict);
            Py_XDECREF(val1);
        }
        PyDict_SetItemString(dict, "key1", val1); 
        Py_DECREF(val1);
        return dict;//PyLong_FromLong(1);
    }
    static PyMethodDef _mymodule_methods[] = {
        {"version", _mymodule_version, METH_VARARGS, "lele.version()"},
        {"foo", _mymodule_foo, METH_VARARGS, "lele.foo(num, str, list, dic, callback)"},
        {"loadConfig", _mymodule_loadConfig, METH_VARARGS, "lele.loadConfig(/path/to/config/json)"},
        {"handleEvents", _mymodule_handleEvents, METH_VARARGS, "lele.handleEvents()"},
        {NULL, NULL, 0, NULL}
    };
    static PyModuleDef _mymodule = {
        PyModuleDef_HEAD_INIT, "lele", NULL, -1, _mymodule_methods,
        NULL, NULL, NULL, NULL
    };
}//namespace

PyMODINIT_FUNC PyInit_lele(void) {
    if(!_graphics_backend.load()) {
        LOG(FATAL, LVSIM, "Failed to load graphcis backend\n")
        return nullptr;
    }
    return PyModule_Create(&_mymodule);
}    
