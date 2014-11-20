#include "ModbusDevices.h"
#include <QtUiTools>
#include "Settings.hpp"

QSettings *Settings::m_Settings = NULL;

ModbusDevices::ModbusDevices(QWidget *parent)
    : QMainWindow(parent)
{
  ui.setupUi(this);
  LOG_INFO("START");

  QUiLoader loader;

  QFile file("D:\\_projects\\rio\\util\\ModbusDevices\\ModbusDevices\\Template.ui");
  file.open(QFile::ReadOnly);
  QWidget *formWidget = loader.load(&file);
  file.close();

  ui.tabWidget->clear();
  ui.tabWidget->addTab(formWidget, formWidget->windowTitle());


  SETTINGS->beginGroup("MainWindow");
  restoreGeometry(SETTINGS->value("geometry").toByteArray());
  restoreState(SETTINGS->value("state").toByteArray());
  ui.splitter->restoreState(SETTINGS->value("splitter").toByteArray());
  SETTINGS->endGroup();
}

ModbusDevices::~ModbusDevices()
{
  SETTINGS->beginGroup("MainWindow");
  SETTINGS->setValue("geometry", saveGeometry());
  SETTINGS->setValue("state", saveState());
  SETTINGS->setValue("splitter", ui.splitter->saveState());
  SETTINGS->sync();
  SETTINGS->endGroup();
}
