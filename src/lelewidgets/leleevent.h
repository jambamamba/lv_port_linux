#pragma once

#include <string>
#include <map>
#include <src/lelewidgets/lelewidgetfactory.h>
#include <Python.h>

// struct lv_event_t;
class LeleEvent {
  public:
  LeleEvent(const std::string &json_str);
  LeleEvent(lv_event_t *e) : _lv_event(e) {}
  LeleEvent(const LeleEvent& rhs, const lv_event_t *e = nullptr, int ivalue = 0);
  const std::string &action() const { return _action; }
  const std::string &type() const { return _type; }
  const std::string &id() const { return _id; }
  void setId(const std::string &id) { _id = id; }
  const lv_event_t *lv_event() const { return _lv_event; }
  const std::map<std::string, std::string> &args() const { return _args; }
  int code() const { return _code; }
  int value() const { return _ivalue; }
  protected:
  void parseArgs(const std::string &json_str);

  std::string _class_name;
  std::string _id;
  std::string _type;
  std::string _action;
  std::map<std::string, std::string> _args;
  const lv_event_t *_lv_event = nullptr;
  int _code = 0;
  int _ivalue = 0;
};


typedef struct {
    PyObject_HEAD
    // Type-specific fields go here
    PyObject *_event_id = nullptr;
    PyObject *_object = nullptr;
    PyObject *_type = nullptr;
    PyObject *_action = nullptr;
    PyObject *_args = nullptr;
    int _event_code = 0;
    int _value = 0;
} PyLeleEvent;

extern PyTypeObject PyLeleEvent_Type;
PyObject *PyLeleEvent_new(PyTypeObject *type, const LeleEvent *lele_event = nullptr, const LeleObject *target_obj = nullptr);
