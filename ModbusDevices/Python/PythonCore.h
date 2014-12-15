#pragma once

#include "IncludeMe.h"
#include <QObject>
#include "include/Python.h"

#define PYTHONCORE  PythonCore::Instance()

class PythonCore : public Singleton<PythonCore>, public QObject
{
  friend Singleton<PythonCore>;
public:
  PythonCore();
  ~PythonCore();

  void initPython();
  void runString(const char* str, const char *config = 0);
  PyObject* runFile(const char* fname, const char *config = 0);

protected:
  void timerEvent(QTimerEvent* e);

private:
  PyObject *_module;
};

