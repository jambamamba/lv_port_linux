#include <filesystem>
#include <iostream>
#include <debug_logger/debug_logger.h>

#include "python_wrapper.h"

LOG_CATEGORY(LVSIM, "LVSIM");

namespace {
    bool _cancel = false;
    static PythonWrapper *_py;

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
        Py_DECREF(list);

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
        Py_DECREF(dic);     

        PyObject *arglist = Py_BuildValue("(s)", "hello from c++");
        PyObject *res = PyObject_CallObject(callback, arglist);
        if(res) { Py_DECREF(res); }
        Py_DECREF(callback);

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
            if(!_py->_loadConfig) {
                std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "No loadConfig was set in load function" << "\n";
                return PyLong_FromLong(-1);
            }
            if(!_py->_loadConfig(input_file)) {
                return PyLong_FromLong(-1);
            }
        }
        return PyLong_FromLong(0);
    }
    static PyObject* _mymodule_runMainLoop(PyObject *self, PyObject *args) {
        if(!_py->_runMainLoop) {
            std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "No runMainLoop was set in load function" << "\n";
            return PyLong_FromLong(-1);
        }
        if(!_py->_runMainLoop()) {
            return PyLong_FromLong(-1);
        }
        return PyLong_FromLong(0);
    }
    static PyMethodDef _mymodule_methods[] = {
        {"version", _mymodule_version, METH_VARARGS, "lele.version()"},
        {"foo", _mymodule_foo, METH_VARARGS, "lele.foo(num, str, list, dic, callback)"},
        {"loadConfig", _mymodule_loadConfig, METH_VARARGS, "lele.loadConfig(/path/to/config/json)"},
        {"runMainLoop", _mymodule_runMainLoop, METH_VARARGS, "lele.runMainLoop()"},
        {NULL, NULL, 0, NULL}
    };
    static PyModuleDef _mymodule = {
        PyModuleDef_HEAD_INIT, "lele", NULL, -1, _mymodule_methods,
        NULL, NULL, NULL, NULL
    };
    static PyObject* PyInitMyModule(void) {
        return PyModule_Create(&_mymodule);
    }    
}//namespace

void PythonWrapper::printError() const
{
   if (!PyErr_Occurred()) {
    return;
   }
    PyObject *type = 0;
    PyObject *value = 0;
    PyObject *traceback = 0;
    PyErr_Fetch(&type, &value, &traceback);
    if (!value) {
        return;
    }
    PyObject *old_str = PyObject_Str(value);
    Py_XINCREF(type);
    PyErr_Clear();
    std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << (PyUnicode_AsUTF8(old_str)) << "\n";
    Py_DECREF(old_str);
}

bool PythonWrapper::callPythonFunction(PyObject *py_module, const char* func, const std::vector<std::string> &args)
{
    PyObject *pFunc = PyObject_GetAttrString(py_module, func);
    if (!pFunc) {
        if (PyErr_Occurred())
        {PyErr_Print();}
        std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "Cannot find function '" << func << "'\n";
        return false;
    }
    else if (!PyCallable_Check(pFunc)) {
        if (PyErr_Occurred())
        {PyErr_Print();}
        std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "This is not a callable function: '" << func << "'\n";
        Py_DECREF(pFunc);
        return false;
    }

    PyObject *pArgs = PyTuple_New(args.size());
    int idx = 0;
    for (const std::string &arg: args) {
        const std::string delimiter(":");
        auto npos = arg.find(delimiter);
        if(npos == arg.npos) {
            std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << " No delimiter found" << "\n";
            break;
        }
        std::string lhs = arg.substr(0, npos);
        std::string rhs = arg.substr(lhs.size() + delimiter.size());
        PyObject *pValue = nullptr;
        if(lhs.at(0) == 's') {
            pValue = PyUnicode_FromString(rhs.c_str());
        }
        else if(lhs.at(0) == 'i') {
            pValue = PyLong_FromLong(std::atoi(rhs.c_str()));
        }

        if (!pValue) {
            Py_DECREF(pFunc);
            Py_DECREF(pArgs);
            std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "Cannot convert argument" << "\n";
            return false;
        }
        // pValue reference stolen here:
        PyTuple_SetItem(pArgs, idx, pValue);
        ++idx;
    }
    PyObject *pValue = PyObject_CallObject(pFunc, pArgs);
    if (!pValue) {
        Py_DECREF(pFunc);
        Py_DECREF(pArgs);
        PyErr_Print();
        std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "Call failed" << "\n";
        return false;
    }

    std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "Result of call: " << PyLong_AsLong(pValue) << "\n";
    Py_DECREF(pFunc);
    Py_DECREF(pArgs);
    Py_DECREF(pValue);

    return true;
}

PyObject *PythonWrapper::loadModule(const std::string &py_script) const
{
    setlocale(LC_ALL, "en_US.UTF-8");
    PyImport_AppendInittab("lele", &PyInitMyModule);

    std::filesystem::path path(py_script);
    std::string py_script_dir = path.parent_path().string();

    PyConfig config;
    PyConfig_InitPythonConfig(&config);
    PyConfig_Read(&config);
    config.module_search_paths_set = 1;
#ifdef WIN32
     extern int Py_NoSiteFlag;
     Py_NoSiteFlag = 1;
#endif
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::string _py_home_dir(std::filesystem::current_path().string() + "/Python");
    std::cout << "Using _py_home_dir: " << _py_home_dir << "\n";
    std::wstring home_ws(converter.from_bytes(_py_home_dir.c_str()));
    PyWideStringList_Append(&config.module_search_paths, home_ws.c_str());
    std::string _py_lib_dir(_py_home_dir + "/Lib");
    std::wstring py_lib_ws(converter.from_bytes(_py_lib_dir.c_str()));
    PyWideStringList_Append(&config.module_search_paths, py_lib_ws.c_str());
    std::wstring py_script_dir_ws(converter.from_bytes(py_script_dir.c_str()));
    PyWideStringList_Append(&config.module_search_paths, py_script_dir_ws.c_str());
    Py_InitializeFromConfig(&config);
 
    //osm todo: use LOG(DEBUG instead of cout
    std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "Loading py script: " << py_script << "\n";
 
   PyObject *obj = PyUnicode_FromString(path.stem().c_str());
   PyObject *py_module =  PyImport_Import(obj);
   Py_DECREF(obj);

   if(!py_module) {
       std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "Failed to load " << py_script.c_str() << "\n";
       printError();
       return nullptr;
   }
    return py_module;
}
PythonWrapper::~PythonWrapper() {
    unload();
}
void PythonWrapper::unload() {
    if(!_py->_py_module) {
        return;
    }
    Py_DECREF(_py->_py_module);
    Py_FinalizeEx();
    _py->_py_module = nullptr;
}
bool PythonWrapper::load(
    const std::string &py_script,
    std::function<bool(const std::string &config_json)> loadConfig,
    std::function<bool()> runMainLoop
    ) {

    static PythonWrapper py;
    _py = &py;
    if(_py->_py_module) {
        std::cout << "Already loaded, refusing to load another script\n";
        return false;
    }
    _py->_py_module = _py->loadModule(py_script);
    if(!_py->_py_module) {
        std::cout << "Failed to load another script: '" << py_script.c_str() << "'\n";
        return false;
    }
    _py->_loadConfig = loadConfig;
    _py->_runMainLoop = runMainLoop;
    return _py->callPythonFunction(_py->_py_module, "main", std::vector<std::string>());
}
