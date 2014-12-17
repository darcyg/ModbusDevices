#include "PythonCore.h"
#include "include/Python.h"
#include <fstream>
#include <memory>

using std::string;

PythonCore::PythonCore() :_module(nullptr)
{
}


PythonCore::~PythonCore()
{
}


static PyObject * py_log(PyObject * self, PyObject * args)
{
  char * msg;
  Py_ssize_t i;
  PyObject *temp_p, *temp_p2;

  if (PyTuple_Check(args))
  {
    auto size = PyTuple_Size(args);
    for (i = 0; i < size; i++) {
      temp_p = PyTuple_GetItem(args, i);
      if (temp_p == NULL)
        return NULL;

      PyObject *str = PyObject_Str(temp_p);

      if (PyString_Check(str) != 1)
        continue;

      auto s = PyString_AsString(str);
      if (s[0] == '\n')
        continue;
      LOG_INFO(s);

      Py_DECREF(temp_p);
    }
  }
  else if (PyString_Check(args))
  {
    auto s = PyString_AsString(args);
    if (s[0] != '\n')
      LOG_INFO(s);
  }
  return Py_None;
}

static PyMethodDef HelloMethods[] = {
  { "log", py_log, METH_VARARGS, "Log your message" },
  { "write", py_log, METH_VARARGS, "Print function" },
  { NULL, NULL, 0, NULL }
};

void PythonCore::initPython()
{
  Py_Initialize();
  _module = Py_InitModule("devemu", HelloMethods);
  if (!_module)
    MD_THROW("Py_InitModule failed!")
  PySys_SetObject("stdout", _module);
  PySys_SetObject("stderr", _module);
}

bool PythonCore::registerFunction(const char* name, PyCFunction pyfunc)
{
  PyMethodDef ml = { name, pyfunc, METH_VARARGS, NULL };
  auto n = PyString_FromString("__main__");
  auto v = PyCFunction_NewEx(&ml, (PyObject*)NULL, n);
  
  PyObject* modules = PyImport_GetModuleDict();
  PyObject* main_mod = PyMapping_GetItemString(modules, "__main__");
  auto d = PyModule_GetDict(main_mod);
  PyDict_SetItemString(d, name, v);
  Py_DECREF(d);
  Py_DECREF(n);
  Py_DECREF(main_mod);
  Py_DECREF(v);
  return true;
}

void PythonCore::runString(const char* str, const char *config)
{
  if (!_module)
    MD_THROW("Python was not initialized!");
  if (PyRun_SimpleString(str) != 0)
    MD_THROW("Script evaluation failed!");
}

void PythonCore::timerEvent(QTimerEvent* e)
{

}

PyObject* PythonCore::runFile(const char* fname, const char *config)
{
  FILE *fp = nullptr;
  fopen_s(&fp, fname, "r");
  fseek(fp, 0, SEEK_END);
  int fsz = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  char *sbuf = new char[fsz + 1];
  sbuf[fread(sbuf, 1, fsz, fp)] = 0;
  if (PyRun_SimpleString(sbuf) != 0)
    MD_THROW("Script evaluation failed!");
  delete[] sbuf;
  fclose(fp);

  PyObject* modules = PyImport_GetModuleDict();
  PyObject* main_mod = PyMapping_GetItemString(modules, "__main__");

  PyObject* attr_str = PyObject_GetAttrString(main_mod, "devcls");

  if (!attr_str)
    LOG_ERROR("FAIL ATTR");
  PyObject* instance = PyObject_CallMethod(main_mod, PyString_AsString(attr_str), "s", config);
  Py_DECREF(main_mod);
  Py_DECREF(modules);
  Py_DECREF(attr_str);
  if (!instance)
  {
    LOG_ERROR("PYTHON: Can not create instance of '%s'", PyString_AsString(attr_str));
    PyErr_PrintEx(0);
    return nullptr;
  }
  py_log(Py_None, PyObject_Str(instance));
  
  return instance;
}