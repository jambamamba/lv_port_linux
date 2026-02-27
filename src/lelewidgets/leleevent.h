#pragma once

#include <string>
#include <map>
#include <src/lelewidgets/lelewidgetfactory.h>
#include <Python.h>

// struct lv_event_t;
class LeleEvent {
public:
  LeleEvent(const std::string &json_str = "");
  virtual bool fromJson(const std::string &json_str);
  LeleEvent(lv_event_t *e, LeleObject *target_obj) : _lv_event(e), _target_obj(target_obj) {}
  LeleEvent(const LeleEvent& rhs, const lv_event_t *e = nullptr, int ivalue = 0, int ivalue2 = 0);
  const std::string &getAction() const { return _action; }
  const std::string &getType() const { return _type; }
  const std::string &getId() const { return _id; }
  void setId(const std::string &id) { _id = id; }
  const lv_event_t *getLvEvent() const { return _lv_event; }
  LeleObject *getTargetObj() const { return _target_obj; }
  void setTargetObj(LeleObject *target_obj) { _target_obj = target_obj; }
  virtual PyObject *createPyObject();
  const std::map<std::string, std::string> &getArgs() const { return _args; }
  enum Type {
      Clicked = 10,
      ValueChanged = 35
    };
  int getValue() const { return _ivalue; }
protected:
  void parseArgs(const std::string &json_str);

  std::string _id;
  std::string _type;
  std::string _action;
  std::map<std::string, std::string> _args;
  const lv_event_t *_lv_event = nullptr;
  LeleObject *_target_obj = nullptr;
  int _code = 0;
  int _ivalue = 0;
  int _ivalue2 = 0;
};


struct PyLeleEvent {
    PyObject ob_base;
    static PyTypeObject _obj_type;
    static PyTypeObject _enum_type;
    static PyMemberDef _members[];
    static PyMethodDef _methods[];
    static void dealloc(PyLeleEvent* self);
    static int init(PyLeleEvent *self, PyObject *args, PyObject *kwds);
    // Type-specific fields go here
    PyObject *_id = nullptr;
    PyObject *_object = nullptr;
    PyObject *_type = nullptr;
    PyObject *_action = nullptr;
    PyObject *_args = nullptr;
    PyObject *_code = nullptr;
    // int _code = 0;
    int _ivalue = 0;
    int _ivalue2 = 0;
};

#define PY_LELEEVENT_MEMBERS() \
    {"id", Py_T_OBJECT_EX, offsetof(PyLeleEvent, _id), 0, "id"},\
    {"object", Py_T_OBJECT_EX, offsetof(PyLeleEvent, _object), 0, " "},\
    {"Type", Py_T_OBJECT_EX, offsetof(PyLeleEvent, _type), 0, "Type"},\
    {"action", Py_T_OBJECT_EX, offsetof(PyLeleEvent, _action), 0, "action"},\
    {"args", Py_T_OBJECT_EX, offsetof(PyLeleEvent, _args), 0, "args"},\
    {"code", Py_T_OBJECT_EX, offsetof(PyLeleEvent, _code), 0, "code"},\
    {"value", Py_T_INT, offsetof(PyLeleEvent, _ivalue), 0, "value"},\
    {"value2", Py_T_INT, offsetof(PyLeleEvent, _ivalue2), 0, "value2"},\

#define PY_LELEEVENT_METHODS() \

///////////////////////////////////////////////////
struct PyLeleEventType {
    PyObject ob_base;
    static PyTypeObject _obj_type;
    static PyMemberDef _members[];
    static int init(PyObject *self, PyObject *args, PyObject *kwds);
    static void dealloc(PyObject* self);
    // Type-specific fields go here
    PyObject *_clicked = nullptr;
    PyObject *_value_changed = nullptr;
};

#define PY_LELEEVENT_TYPE_MEMBERS() \
    {"Clicked", Py_T_OBJECT_EX, offsetof(PyLeleEventType, _clicked), 0, "Clicked"},\
    {"ValueChanged", Py_T_INT, offsetof(PyLeleEventType, _value_changed), 0, "ValueChanged"},

