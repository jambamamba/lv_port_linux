#include <python/python_helper.h>

#include <debug_logger/debug_logger.h>

LOG_CATEGORY(LVSIM, "LVSIM");

std::unordered_set<std::string> pyListToStringSet(PyObject *args) {
    std::unordered_set<std::string> strings;
    Py_ssize_t num_args = PyTuple_Size(args);
    if(num_args != 1) {
        return strings;
    }
    PyObject *obj = nullptr;
    if(!PyArg_ParseTuple(args, "O", //list
                &obj)) {
        LOG(FATAL, LVSIM, "Failed to parse args\n");
        return strings;
    }
    if (PyUnicode_Check(obj)){
        const char *item = PyUnicode_AsUTF8(obj);
        if(item) {
            strings.insert(item);
            return strings;
        }
    }
    if (!PyList_Check(obj)) {
        LOG(WARNING, LVSIM, "Is not list type!\n");
        Py_XDECREF(obj);
        return strings;
    }
    if (!Py_IS_TYPE(obj, &PyList_Type)) {
        LOG(WARNING, LVSIM, "Is not list type!\n");
        Py_XDECREF(obj);
        return strings;
    }
    int len = PyList_Size(obj);
    while (len--) {
        PyObject *item = PyList_GetItem(obj, len);
        if(!item) {
            LOG(WARNING, LVSIM, "Could not parse list item!\n");
            continue;
        }
        if(!PyUnicode_Check(item)) {
        // if (!Py_IS_TYPE(obj, &PyUnicode_Type)) {
            LOG(WARNING, LVSIM, "Is not string type!\n");
            Py_XDECREF(item);
            continue;
        }
        const char *item_ = PyUnicode_AsUTF8(item);
        if(!item_) {
            int num = PyList_Size(obj);
            LOG(WARNING, LVSIM, "Could not parse list item!\n");
            Py_XDECREF(obj);
            continue;
        }
        strings.insert(item_);
    }
    Py_XDECREF(obj);
    return strings;
}

std::vector<std::string> pyListToStringVector(PyObject *args) {
    std::vector<std::string> strings;
    Py_ssize_t num_args = PyTuple_Size(args);
    if(num_args != 1) {
        return strings;
    }
    PyObject *obj = nullptr;
    if(!PyArg_ParseTuple(args, "O", //list
                &obj)) {
        LOG(FATAL, LVSIM, "Failed to parse args\n");
        return strings;
    }
    if (PyUnicode_Check(obj)){
        const char *item = PyUnicode_AsUTF8(obj);
        if(item) {
            strings.push_back(item);
            return strings;
        }
    }
    if (!PyList_Check(obj)) {
        LOG(WARNING, LVSIM, "Is not list type!\n");
        Py_XDECREF(obj);
        return strings;
    }
    if (!Py_IS_TYPE(obj, &PyList_Type)) {
        LOG(WARNING, LVSIM, "Is not list type!\n");
        Py_XDECREF(obj);
        return strings;
    }
    int len = PyList_Size(obj);
    while (len--) {
        PyObject *item = PyList_GetItem(obj, len);
        if(!item) {
            LOG(WARNING, LVSIM, "Could not parse list item!\n");
            continue;
        }
        if(!PyUnicode_Check(item)) {
        // if (!Py_IS_TYPE(obj, &PyUnicode_Type)) {
            LOG(WARNING, LVSIM, "Is not string type!\n");
            Py_XDECREF(item);
            continue;
        }
        const char *item_ = PyUnicode_AsUTF8(item);
        if(!item_) {
            int num = PyList_Size(obj);
            LOG(WARNING, LVSIM, "Could not parse list item!\n");
            Py_XDECREF(obj);
            continue;
        }
        strings.push_back(item_);
    }
    Py_XDECREF(obj);
    return strings;
}