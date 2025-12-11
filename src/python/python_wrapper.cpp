#include <filesystem>
#include <iostream>
#include <debug_logger/debug_logger.h>

#include <lelewidgets/leleevent.h>
#include <lelewidgets/leleobject.h>

#include "python_wrapper.h"

LOG_CATEGORY(LVSIM, "LVSIM");

namespace {
    static PythonWrapper *_py;
}//namespace

PyMODINIT_FUNC PyInit_lele(void);

void PythonWrapper::printError() const {
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
    if (traceback) {
        PyTraceBack_Print(traceback, PySys_GetObject("stderr"));
    }
    Py_XINCREF(type);
    PyErr_Clear();
    // std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << (PyUnicode_AsUTF8(old_str)) << "\n";
    LOG(WARNING, LVSIM, "%s\n", PyUnicode_AsUTF8(old_str));
    PyErr_Print();
    Py_DECREF(old_str);
}

bool PythonWrapper::callPythonFunction(PyObject *py_module, const char* func, const std::vector<std::string> &args) {
    PyObject *pFunc = PyObject_GetAttrString(py_module, func);
    if (!pFunc) {
        if (PyErr_Occurred())
        {PyErr_Print();}
        // std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "Cannot find function '" << func << "'\n";
        LOG(WARNING, LVSIM, "Cannot find function '%s'\n", func);
        return false;
    }
    else if (!PyCallable_Check(pFunc)) {
        if (PyErr_Occurred())
        {PyErr_Print();}
        // std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "This is not a callable function: '" << func << "'\n";
        LOG(WARNING, LVSIM, "This is not a callable function: '%s'\n", func);
        Py_DECREF(pFunc);
        return false;
    }

    PyObject *pArgs = PyTuple_New(args.size());
    int idx = 0;
    for (const std::string &arg: args) {
        const std::string delimiter(":");
        auto npos = arg.find(delimiter);
        if(npos == arg.npos) {
            // std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << " No delimiter found" << "\n";
            LOG(WARNING, LVSIM, "No delimiter found\n");
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
            // std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "Cannot convert argument" << "\n";
            LOG(WARNING, LVSIM, "Cannot convert argument\n");
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
        // std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "Call failed" << "\n";
        LOG(WARNING, LVSIM, "Call failed\n");
        return false;
    }

    // std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "Result of call: " << PyLong_AsLong(pValue) << "\n";
    LOG(WARNING, LVSIM, "Result of call: %d\n", PyLong_AsLong(pValue));
    Py_DECREF(pFunc);
    Py_DECREF(pArgs);
    Py_DECREF(pValue);

    return true;
}

PyObject *PythonWrapper::loadModule(const std::string &py_script) const {
    setlocale(LC_ALL, "en_US.UTF-8");
    PyImport_AppendInittab("lele", &PyInit_lele);

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
    const char* PYTHONHOME = std::getenv("PYTHONHOME"); 
    LOG(DEBUG, LVSIM, "PYTHONHOME: %s\n", PYTHONHOME);
    std::string _py_home_dir(
        PYTHONHOME ? 
            PYTHONHOME : 
            std::filesystem::current_path().string() + "/Python");
    LOG(DEBUG, LVSIM, "Using _py_home_dir: %s\n", _py_home_dir.c_str());

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring home_ws(converter.from_bytes(_py_home_dir.c_str()));
    PyWideStringList_Append(&config.module_search_paths, home_ws.c_str());
    std::string _py_lib_dir(_py_home_dir + "/Lib");
    std::wstring py_lib_ws(converter.from_bytes(_py_lib_dir.c_str()));
    PyWideStringList_Append(&config.module_search_paths, py_lib_ws.c_str());
    std::wstring py_script_dir_ws(converter.from_bytes(py_script_dir.c_str()));
    PyWideStringList_Append(&config.module_search_paths, py_script_dir_ws.c_str());
    Py_InitializeFromConfig(&config);
 
    // std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "Loading py script: " << py_script << "\n";
    LOG(DEBUG, LVSIM, "Loading py script: '%s'\n", py_script.c_str());
 
   PyObject *obj = PyUnicode_FromString(path.stem().c_str());
   PyObject *py_module =  PyImport_Import(obj);
   Py_DECREF(obj);

   if(!py_module) {
    //    std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "Failed to load " << py_script.c_str() << "\n";
       LOG(WARNING, LVSIM, "Failed to load: '%s'\n", py_script.c_str());
    //    printPyBacktrace();
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
    std::function<bool(const std::string &config_json)> loadConfig
    ) {

    static PythonWrapper py;
    if(py._py_module) {
        // std::cout << "Already loaded, refusing to load another script\n";
        LOG(WARNING, LVSIM, "Already loaded, refusing to load another script\n");
        return false;
    }
    py._py_module = py.loadModule(py_script);
    if(!py._py_module) {
        // std::cout << "Failed to load another script: '" << py_script.c_str() << "'\n";
        LOG(WARNING, LVSIM, "Failed to load the script: '%s'\n", py_script.c_str());
        return false;
    }
    _py = &py;
    // if(!py.callPythonFunction(py._py_module, "main", std::vector<std::string>())) {
    //     return false;
    // }
    return true;    
}
