#include <debug_logger/debug_logger.h>
#include <filesystem>
#include <iostream>
#include <lelewidgets/lelebutton.h>
#include <lelewidgets/leleevent.h>
#include <lelewidgets/leleimage.h>
#include <lelewidgets/lelemessagebox.h>
#include <lelewidgets/leleobject.h>
#include <lelewidgets/lelerollerview.h>
#include <mandelbrot/mandelbrot.h>
#include <tr/tr.h>
#include <lvgl/lvgl.h>

#include "python_helper.h"
#include "python_wrapper.h"
#include "graphics_backend.h"

LOG_CATEGORY(LVSIM, "LVSIM");

namespace {
    GraphicsBackend &_graphics_backend = GraphicsBackend::getInstance();
    static std::vector<std::pair<std::string, LeleWidgetFactory::Node>> _nodes;

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
    std::string getPyScriptDir() {
        std::string code =
        "import os\n"
        "res = os.getcwd()\n"
        "";
        
        RAII $;
        $.global_dict = PyDict_New();
        if (!$.global_dict) { return ""; }
        $.local_dict = PyDict_New();
        if (!$.local_dict) { return ""; }
        $.ret = PyRun_String(code.c_str(), Py_file_input, $.global_dict, $.local_dict);

        if (!$.ret) { 
            LOG(WARNING, LVSIM, "Failed in PyRun_String!\n");
            return ""; 
        }
        PyObject *output = PyDict_GetItemString($.local_dict, "res");
        if (!output) {
            // PyDict_GetItemString does not set exceptions
            LOG(WARNING, LVSIM, "Failed in PyDict_GetItemString!\n");
            PyErr_SetString(PyExc_KeyError, "could not get 'res'");
            return "";
        } else {
            Py_INCREF(output); // PyDict_GetItemString returns a borrow reference
        }

        const char *path = PyUnicode_AsUTF8(output);
        if(!path) {
            LOG(WARNING, LVSIM, "Failed in PyUnicode_AsUTF8!\n");
            return "";
        }

        return path;
    }
#ifdef MULTI_PHASE_INIT
    static PyObject* createPyModule();

    static PyObject* _mymodule_exec(PyObject* spec) {
        LOG(FATAL, LVSIM, "_mymodule_exec\n");
        return nullptr;
    }
    static PyObject* _mymodule_create(PyObject* spec) {
        PyObject *module = createPyModule();
        return module;
    }
#endif//MULTI_PHASE_INIT
    //Functions that py script can call
    static PyObject* _mymodule_version(PyObject *self, PyObject *args) {
        int major_version = LELE_VERSION_MAJOR;//(int)(This->GetVersion().toFloat());
        int minor_version = LELE_VERSION_MINOR;//(int)((This->GetVersion().toFloat() - major_version) * 100.);
        PyObject *dict = PyDict_New();
        PyDict_SetItem(dict, PyUnicode_FromString("Major"), PyLong_FromDouble(major_version));
        PyDict_SetItem(dict, PyUnicode_FromString("Minor"), PyLong_FromDouble(minor_version));
        return dict;
    }
    static PyObject* _mymodule_addEventHandler(PyObject *self, PyObject *args) {
        char *id = nullptr;
        PyObject *py_callback = nullptr;
        if(!PyArg_ParseTuple(args, "sO", //id, obj
            &id,
            &py_callback)) {
            return PyBool_FromLong(false);
        }
        // LOG(DEBUG, LVSIM, "@@@> _mymodule_addEventHandler id:'%s'\n", id);
        LeleWidgetFactory::iterateNodes(_nodes, 0, [id, py_callback](LeleObject &lele_object) {
            if(lele_object.getId() == id) {
                Py_XINCREF(py_callback);
                lele_object.addEventHandler(py_callback);
            }
        });
        return PyBool_FromLong(true);
    }
    static PyObject* _mymodule_getObjectById(PyObject *self, PyObject *args) {
        char *id = nullptr;
        if(!PyArg_ParseTuple(args, "s", //id
            &id)) {
            return Py_None;
        }
        std::vector<PyObject*> py_objects;
        LeleWidgetFactory::iterateNodes(_nodes, 0, [id,&py_objects](LeleObject &lele_object) {
            if(lele_object.getId() == id) {
                PyObject *py_obj = lele_object.createPyObject();
                if(!py_obj){
                    LOG(FATAL, LVSIM, "Could not create PyObject!\n");
                    return;
                }
                py_objects.emplace_back(py_obj);
            }
        });
        return PyLeleObject::pyListOrPyObjectFromStdVector(py_objects);
    }
    // static PyObject* _mymodule_foo(PyObject *self, PyObject *args) {
    //     int num = 0;
    //     char *str = nullptr;
    //     PyObject *list = nullptr;
    //     PyObject *dict = nullptr;
    //     PyObject *py_callback = nullptr;
    //     if(!PyArg_ParseTuple(args, "isOOO", //int,str,obj,obj,obj
    //         &num,
    //         &str,
    //         &list,
    //         &dict,
    //         &py_callback)) {
    //         return Py_None;
    //     }
    //     std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "num: " << num << "\n";
    //     std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "str: " << str << "\n";

    //     int len = PyList_Size(list);
    //     while (len--) {
    //         std::string str(PyUnicode_AsUTF8(PyList_GetItem(list, len)));
    //         std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "List item: " << str << "\n";
    //     }
    //     // Py_DECREF(list);

    //     PyObject* keys = PyDict_Keys(dict); 
    //     Py_ssize_t size = PyList_Size(keys);
    //     for (Py_ssize_t i = 0; i < size; i++) {
    //         PyObject* key = PyList_GetItem(keys, i);
    //         PyObject* key_str = PyObject_Str(key);
    //         const char* key_c_str = PyUnicode_AsUTF8(key_str);
    //         PyObject *value_str = PyDict_GetItemString(dict, key_c_str);
    //         const char* value_c_str = PyUnicode_AsUTF8(value_str);
    //         std::cout << "[PY]" << __FILE__ << ":" << __LINE__ << " " << "Dict key: " << key_c_str << ", value: " << value_c_str << "\n";
    //         Py_DECREF(key_str);
    //         Py_DECREF(value_str);
    //     }
    //     // Py_DECREF(dic);     

    //     PyObject *arglist = Py_BuildValue("(s)", "hello from c++");
    //     PyObject *res = PyObject_CallObject(py_callback, arglist);
    //     if(res) { Py_DECREF(res); }
    //     // Py_DECREF(py_callback);

    //     return PyLong_FromLong(1);
    // }
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

        std::string input_file(str);
        if(*str && str[0] != '/') {
            while(true) {
                std::string path = std::filesystem::current_path().string() + "/" + str;
                LOG(DEBUG, LVSIM, "Try to load config: '%s'\n", path.c_str());
                if(std::filesystem::exists(path)) {
                    input_file = path;
                    break;
                }
                std::string script_dir = getPyScriptDir();
                if(std::filesystem::current_path().string() != script_dir) {
                    path = script_dir + "/" + str;
                    LOG(DEBUG, LVSIM, "Try to load config: '%s'\n", path.c_str());
                    if(std::filesystem::exists(path)) {
                        input_file = path;
                        break;
                    }
                }
                LOG(WARNING, LVSIM, "Failed to load config, file not found: '%s'\n", str);
                return PyBool_FromLong(false);
            }
        }

        LOG(DEBUG, LVSIM, "Loading config: '%s'\n", input_file.c_str());
        static LeleObject _root(nullptr);
        _nodes = LeleWidgetFactory::fromConfig(&_root, input_file);
        if(_nodes.size() == 0) {
            LOG(WARNING, LVSIM, "Failed to load config: '%s'\n", input_file.c_str());
            return PyBool_FromLong(false);
        }

        return PyBool_FromLong(true);
    }
    static PyObject* _mymodule_hasCuda(PyObject *self, PyObject *args) {
        return PyBool_FromLong(Mandelbrot::hasCuda());
    }
    static PyObject* _mymodule_generateMandelbrot(PyObject *self, PyObject *args) {
        const char *filename = nullptr;
        int width = 800, height = 600, max_iterations = 1000;
        double escape_radius_sq = 4.0;
        double real_min = -2.0, real_max = 1.0;
        double imag_min = -1.0, imag_max = 1.0;
        int color_scheme = Mandelbrot::ClassicBlue;
        Py_ssize_t nargs = PyTuple_Size(args);
        if(nargs < 1) {
            LOG(WARNING, LVSIM, "generateMandelbrot: need at least 1 arg\n");
            return PyBool_FromLong(false);
        }
        PyObject *p0 = PyTuple_GetItem(args, 0);
        if(!p0 || !PyUnicode_Check(p0)) {
            LOG(WARNING, LVSIM, "generateMandelbrot: arg 0 must be string\n");
            return PyBool_FromLong(false);
        }
        filename = PyUnicode_AsUTF8(p0);
        if(nargs >= 2) width        = (int)PyLong_AsLong(PyTuple_GetItem(args, 1));
        if(nargs >= 3) height       = (int)PyLong_AsLong(PyTuple_GetItem(args, 2));
        if(nargs >= 4) max_iterations = (int)PyLong_AsLong(PyTuple_GetItem(args, 3));
        if(nargs >= 5) escape_radius_sq = PyFloat_AsDouble(PyTuple_GetItem(args, 4));
        if(nargs >= 6) real_min     = PyFloat_AsDouble(PyTuple_GetItem(args, 5));
        if(nargs >= 7) real_max     = PyFloat_AsDouble(PyTuple_GetItem(args, 6));
        if(nargs >= 8) imag_min     = PyFloat_AsDouble(PyTuple_GetItem(args, 7));
        if(nargs >= 9) imag_max     = PyFloat_AsDouble(PyTuple_GetItem(args, 8));
        if(nargs >= 10) {
            PyObject *p9 = PyTuple_GetItem(args, 9);
            if(p9 && PyLong_Check(p9)) color_scheme = (int)PyLong_AsLong(p9);
        }
        LOG(DEBUG, LVSIM, "generateMandelbrot: parsed color_scheme=%d\n", color_scheme);
        bool ok = Mandelbrot::generateToFile(
            filename, width, height, max_iterations, escape_radius_sq,
            real_min, real_max, imag_min, imag_max,
            static_cast<Mandelbrot::ColorScheme>(color_scheme));
        return PyBool_FromLong(ok);
    }
    static PyObject* _mymodule_handleEvents(PyObject *self, PyObject *args) {
        if(!_graphics_backend.handleEvents()) {
            return PyLong_FromLong(0);
        }
        return PyLong_FromLong(1);
    }
    static PyObject* _mymodule_updateMandelbrotImage(PyObject *self, PyObject *args) {
        const char *img_id = nullptr;
        int max_iterations = 1000;
        double escape_radius_sq = 4.0;
        double real_min = -2.0, real_max = 1.0;
        double imag_min = -1.0, imag_max = 1.0;
        int color_scheme = Mandelbrot::ClassicBlue;
        // Parse directly from tuple to avoid format string/count issues
        Py_ssize_t nargs = PyTuple_Size(args);
        LOG(DEBUG, LVSIM, "updateMandelbrotImage: nargs=%zd\n", nargs);
        if(nargs < 1) {
            LOG(WARNING, LVSIM, "updateMandelbrotImage: need at least 1 arg\n");
            return PyBool_FromLong(false);
        }
        PyObject *p0 = PyTuple_GetItem(args, 0);
        if(!p0 || !PyUnicode_Check(p0)) {
            LOG(WARNING, LVSIM, "updateMandelbrotImage: arg 0 must be string\n");
            return PyBool_FromLong(false);
        }
        img_id = PyUnicode_AsUTF8(p0);
        if(nargs >= 2) max_iterations = (int)PyLong_AsLong(PyTuple_GetItem(args, 1));
        if(nargs >= 3) escape_radius_sq = PyFloat_AsDouble(PyTuple_GetItem(args, 2));
        if(nargs >= 4) real_min = PyFloat_AsDouble(PyTuple_GetItem(args, 3));
        if(nargs >= 5) real_max = PyFloat_AsDouble(PyTuple_GetItem(args, 4));
        if(nargs >= 6) imag_min = PyFloat_AsDouble(PyTuple_GetItem(args, 5));
        if(nargs >= 7) imag_max = PyFloat_AsDouble(PyTuple_GetItem(args, 6));
        if(nargs >= 8) {
            PyObject *p7 = PyTuple_GetItem(args, 7);
            if(p7 && PyLong_Check(p7)) color_scheme = (int)PyLong_AsLong(p7);
        }

        LeleImage *target_img = nullptr;
        LeleWidgetFactory::iterateNodes(_nodes, 0, [img_id, &target_img](LeleObject &lele_object) {
            if(lele_object.getId() == img_id) {
                target_img = dynamic_cast<LeleImage*>(&lele_object);
            }
        });
        if(!target_img) {
            LOG(WARNING, LVSIM, "updateMandelbrotImage: could not find image with id: %s\n", img_id);
            return PyBool_FromLong(false);
        }

        auto [w, h] = target_img->getSize();
        if(w <= 0 || h <= 0) {
            LOG(WARNING, LVSIM, "updateMandelbrotImage: invalid dims: %dx%d\n", w, h);
            return PyBool_FromLong(false);
        }

        LOG(DEBUG, LVSIM, "updateMandelbrotImage: generating %dx%d (real=[%0.3f,%0.3f] imag=[%0.3f,%0.3f]) scheme=%d\n",
            w, h, real_min, real_max, imag_min, imag_max, color_scheme);
        auto bgr = Mandelbrot::generateBGR(w, h, max_iterations, escape_radius_sq,
                                           real_min, real_max, imag_min, imag_max,
                                           static_cast<Mandelbrot::ColorScheme>(color_scheme));
        if(bgr.empty()) {
            LOG(WARNING, LVSIM, "updateMandelbrotImage: generateBGR returned empty!\n");
            return PyBool_FromLong(false);
        }
        LOG(DEBUG, LVSIM, "updateMandelbrotImage: generated %zu bytes\n", bgr.size());

        bool ok = target_img->setBGRBuffer(w, h, bgr);
        LOG(DEBUG, LVSIM, "updateMandelbrotImage: setBGRBuffer returned %d\n", (int)ok);
        return PyBool_FromLong(ok);
    }
    static PyObject* _mymodule_dumpScreenshot(PyObject *self, PyObject *args) {
        GraphicsBackend::getInstance().dumpScreenshot();
        Py_RETURN_NONE;
    }
    static PyMethodDef _mymodule_methods[] = {
        {"version", _mymodule_version, METH_VARARGS, "lele.version()"},
        // {"foo", _mymodule_foo, METH_VARARGS, "lele.foo(num, str, list, dic, callback)"},
        {"loadConfig", _mymodule_loadConfig, METH_VARARGS, "lele.loadConfig(/path/to/config/json)"},
        {"handleEvents", _mymodule_handleEvents, METH_VARARGS, "lele.handleEvents()"},
        {"hasCuda", _mymodule_hasCuda, METH_NOARGS, "lele.hasCuda() -> bool"},
        {"generateMandelbrot", _mymodule_generateMandelbrot, METH_VARARGS, "lele.generateMandelbrot(filename, width, height, ..., color_scheme) -> bool"},
        {"addEventHandler", _mymodule_addEventHandler, METH_VARARGS, "lele.addEventHandler(callback)"},
        {"getObjectById", _mymodule_getObjectById, METH_VARARGS, "lele.getObjectById(id)"},
        {"dumpScreenshot", _mymodule_dumpScreenshot, METH_NOARGS, "lele.dumpScreenshot()"},
        {"updateMandelbrotImage", _mymodule_updateMandelbrotImage, METH_VARARGS, "lele.updateMandelbrotImage(img_id, max_iterations, escape_radius_sq, real_min, real_max, imag_min, imag_max, color_scheme) -> bool"},
        {nullptr, nullptr, 0, nullptr} // Sentinel
    };
#ifdef MULTI_PHASE_INIT
    static PyModuleDef_Slot _mymodule_slots[] = {
        {Py_mod_create, (void*)_mymodule_create},
        {Py_mod_exec, (void*) _mymodule_exec},
        {0, nullptr} // Sentinel to mark the end of the array
    };    
#endif
    static PyModuleDef _mymodule = {
        PyModuleDef_HEAD_INIT, //m_base
        "lele", //m_name
        nullptr, //m_doc
        0, //m_size // m_size must be non-negative (0) for multi-phase init, and can be -1 
        _mymodule_methods, //m_methods
#ifdef MULTI_PHASE_INIT
        _mymodule_slots, //m_slots //PyModule_Create is incompatible with m_slots
#else
        nullptr, //m_slots
#endif
        nullptr, //m_traverse
        nullptr, //m_clear
        nullptr //m_free
    };
#ifdef MULTI_PHASE_INIT
    static PyObject* createPyModule() {
        // PyObject *module_spec = PyObject_CallFunctionObjArgs((PyObject *)&PyModuleSpec_Type, nullptr);

        // PyObject *module_spec = PyObject_GetAttr(mod, &_Py_ID(__spec__));
        PyObject* module_name = PyUnicode_FromString("lele");

        PyObject *module_spec = Py_BuildValue("s", module_name);
        PyObject_SetAttrString(module_spec, "name", module_name);
        Py_DECREF(module_name); // Decrement reference count for the name object

        PyObject *script_dir = PyUnicode_FromString("/repos/lv_port_linux/src/examples/hello-world/hello-world.py");
        PyObject_SetAttrString(module_spec, "__file__", script_dir);
        Py_DECREF(script_dir);

        PyObject *module = PyModule_FromDefAndSpec(&_mymodule, module_spec);
        PyModule_ExecDef(module, &_mymodule);

        // PyModule_AddObject(mod, "foo", PyUnicode_FromString("bar"));//will show up in Python as lele.foo with value "bar"
        return module;
    }
#endif//MULTI_PHASE_INIT
}//namespace

PyObject *getPyModule() {
    static PyObject *module = PyModule_Create(&_mymodule);
    return module;
}

PyObject *getEnumModule() {
    static PyObject *module = PyImport_ImportModule("enum");
    return module;
}

/////////////////////////////////////////////////////////////////////
PyMODINIT_FUNC PyInit_lele(void) {
    // if(!_graphics_backend.load()) {
    //     LOG(FATAL, LVSIM, "Failed to load graphcis backend\n");
    //     return nullptr;
    // }
#ifdef MULTI_PHASE_INIT
    PyObject *module = PyModuleDef_Init(&_mymodule);//leads to _mymodule_create
#else
    PyObject *enum_mod = getEnumModule();
    PyObject *module = getPyModule();
#endif
    // PyModule_AddObject(module, "foo", PyUnicode_FromString("bar"));//will show up in Python as lele.foo with value "bar"
    PyModule_AddType(module, &PyLeleObject::_obj_type);
    PyModule_AddType(module, &PyLeleLabel::_obj_type);
    PyModule_AddType(module, &PyLeleMessageBox::_obj_type);
    PyModule_AddType(module, &PyLeleStyle::_obj_type);
    PyModule_AddType(module, &PyLeleEvent::_obj_type);
    PyModule_AddType(module, &PyLeleEventType::_obj_type);
    PyModule_AddType(module, &PyLeleButton::_obj_type);
    PyModule_AddType(module, &PyLeleButtonType::_obj_type);
    PyModule_AddType(module, &PyLeleFont::_obj_type);
    PyModule_AddType(module, &PyLeleLanguage::_obj_type);
    PyModule_AddType(module, &PyLeleRollerView::_obj_type);
    // PyModule_AddObject(module, "FooBar", 
    //     LeleObject::createPyEnum("FooBar", {
    //             {"FOO",1},
    //             {"BAR",2}
    //         }
    //     )
    // );


    return module;
}