#pragma once

#include <Python.h>
#include <string>
#include <unordered_set>
#include <vector>

std::unordered_set<std::string> pyListToStringSet(PyObject *args);
std::vector<std::string> pyListToStringVector(PyObject *args);