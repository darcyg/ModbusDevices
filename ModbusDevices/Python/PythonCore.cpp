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
  
  //string sfname(fname);
  //size_t np = sfname.rfind('/');
  //if (np == string::npos)
  //  np = sfname.rfind('\\');
  //if (np == string::npos)
  //  np = 0;
  //else np++;
  //string smod = string(fname).substr(np, sfname.rfind('.') != string::npos ? sfname.rfind('.') - np : sfname.length() - np);

  PyObject* modules = PyImport_GetModuleDict();
  sbuf[fread(sbuf, 1, fsz, fp)] = 0;
  PyObject* main_mod = PyMapping_GetItemString(modules, "__main__");
  //PyObject* mod = PyModule_New(smod.c_str());
  //mod = PyImport_AddModule(smod.c_str());
  //PyObject* py_dict = PyModule_GetDict(mod);
  //PyObject* main_dict = PyModule_GetDict(mod);
  //PyObject* prun = PyRun_String("temp  = 9\nprint 'ASDASD %s'%temp", Py_file_input, py_dict, py_dict);
  if (PyRun_SimpleString(sbuf) != 0)
    MD_THROW("Script evaluation failed!");
  delete[] sbuf;
  fclose(fp);
  
  
  
  //PyObject* instance = PyObject_CallMethod(main_mod, "MyClass", "");
  PyObject* attr = PyObject_GetAttrString(main_mod, "device_class");
  PyObject* attr_str = PyObject_GetAttrString(main_mod, "devcls");

  //py_log(nullptr, attr);
  //PyObject_Print(attr, stdout, Py_PRINT_RAW);

  if (!attr_str)
    LOG_ERROR("FAIL ATTR");
  PyObject* instance = PyObject_CallMethod(main_mod, PyString_AsString(attr_str), "s", config);
  if (!instance)
  {
    LOG_ERROR("PYTHON: Can not create instance of '%s'", PyString_AsString(attr_str));
    PyErr_PrintEx(0);
    return nullptr;
  }
  py_log(Py_None, PyObject_Str(instance));
  //PyObject_Print(PyObject_Str(instance), stdout, Py_PRINT_RAW);
  //PyObject* instance2 = PyObject_CallMethod(main_mod, PyString_AsString(attr_str), "");
  
  /*
  for (int i = 0; i < PyList_Size(PyDict_Values(modules)); i++)
  {
    PyObject *pitem = PyList_GetItem(mod_list, i);
    PyObject *str = PyObject_Str(pitem);

    auto s = PyString_AsString(str);
    LOG_INFO(s);

    //PyObject_Print(pitem, stdout, Py_PRINT_RAW);
  }
  */
  return instance;
}