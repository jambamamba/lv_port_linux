#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <Python.h>

#include <debug_logger/debug_logger.h>

LOG_CATEGORY(LVSIM, "LVSIM");

namespace {
    bool _cancel = false;

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
    static PyMethodDef _mymodule_methods[] = {
        {"version", _mymodule_version, METH_VARARGS, "mymodule.version()"},
        {"foo", _mymodule_foo, METH_VARARGS, "mymodule.foo(num, str, list, dic, callback)"},
        {NULL, NULL, 0, NULL}
    };
    static PyModuleDef _mymodule = {
        PyModuleDef_HEAD_INIT, "mymodule", NULL, -1, _mymodule_methods,
        NULL, NULL, NULL, NULL
    };
    static PyObject* PyInitMyModule(void) {
        return PyModule_Create(&_mymodule);
    }    
}//namespace

class PythonWrapper {
    public:
    void printError() const;
    PyObject *loadModule(const std::string &py_script) const;
    void runScript(const std::string &py_script) const;
    bool callPythonFunction(PyObject *py_module, const char* func, const std::vector<std::string> &args);

};//class PythonWrapper


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
    PyImport_AppendInittab("mymodule", &PyInitMyModule);

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
    std::wstring home_ws(converter.from_bytes(_py_home_dir.c_str()));
    PyWideStringList_Append(&config.module_search_paths, home_ws.c_str());
    std::string _py_lib_dir(_py_home_dir + "/Lib");
    std::wstring py_lib_ws(converter.from_bytes(_py_lib_dir.c_str()));
    PyWideStringList_Append(&config.module_search_paths, py_lib_ws.c_str());
    std::wstring py_script_dir_ws(converter.from_bytes(py_script_dir.c_str()));
    PyWideStringList_Append(&config.module_search_paths, py_script_dir_ws.c_str());
    Py_InitializeFromConfig(&config);
 
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
// int main(int argc, char** argv) {
    
//     if(argc != 2) {
//         std::cout << "Please provide absolute path to the python file you want to run\n";
//         return -1;
//     }

//     PythonWrapper wrapper;
//     PyObject *py_module = wrapper.loadModule(argv[1]);
//     if(!py_module) {
//         return -1;
//     }
//     wrapper.callPythonFunction(py_module, "main", std::vector<std::string>());
//     Py_DECREF(py_module);
//     Py_FinalizeEx();

//     return 0;
// }