%module pyspiceql

%include std_string.i
%include std_vector.i

#include <nlohmann/json.hpp>

%typemap(in) nlohmann::json {
  if (PyDict_Check($input)) {
    PyObject* module = PyImport_ImportModule("json");
    PyObject* jsonDumps = PyUnicode_FromString("dumps");
    PyObject* pythonJsonString = PyObject_CallMethodObjArgs(module, jsonDumps, $input, NULL);
    $1 = nlohmann::json::parse(PyUnicode_AsUTF8(pythonJsonString));
  }
  else {
    PyErr_SetString(PyExc_TypeError, "not a dictionary");
    SWIG_fail;
  }
}

// %typemap(out) nlohmann::json {

// }

%include "io.i"
%include "query.i"
%include "spice_types.i"
%include "utils.i"