#pragma once

#include <functional>
#include <string>
#include <vector>
#include <Python.h>

class PythonWrapper {
public:
    ~PythonWrapper();
    static bool load(
        const std::string &py_script,
        std::function<bool(const std::string &config_json)> loadConfig = nullptr,
        std::function<bool(std::function<bool()>)> runMainLoop = nullptr
    );
    static void unload();

    std::function<bool(const std::string &config_json)> _loadConfig = nullptr;
    std::function<bool(std::function<bool()>)> _runMainLoop = nullptr;
protected:
    PythonWrapper() = default;
    PyObject *loadModule(const std::string &py_script) const;
    bool callPythonFunction(PyObject *py_module, const char* func, const std::vector<std::string> &args);
    void printError() const;

    PyObject *_py_module = nullptr;
};//class PythonWrapper

