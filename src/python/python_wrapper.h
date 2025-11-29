#pragma once

#include <functional>
#include <string>
#include <vector>
#include <Python.h>

class LeleEvent;
struct _object;
typedef struct _object PyObject;
class PythonWrapper {
public:
    ~PythonWrapper();
    static bool load(
        const std::string &py_script,
        std::function<bool(const std::string &config_json)> loadConfig = nullptr
    );
    static void unload();
    static void pyCallback(PyObject *py_callback, LeleEvent &&e, const std::string &target_obj_id);
protected:
    PythonWrapper() = default;
    PyObject *loadModule(const std::string &py_script) const;
    bool callPythonFunction(PyObject *py_module, const char* func, const std::vector<std::string> &args);
    void printError() const;

    PyObject *_py_module = nullptr;
};//class PythonWrapper

