#include "DeviceWidget.h"
#include "ModbusDevices.h"
#include "Settings.hpp"
#include <QMessageBox>
#include <QFileDialog>
#include "Python/PythonCore.h"

QSettings *Settings::m_Settings = NULL;

ModbusDevices::ModbusDevices(QWidget *parent)
: QMainWindow(parent)
{
  ui.setupUi(this);
  LOG_INFO("START");

  ui.tabWidget->clear();

  connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(openDevice()));
  connect(ui.actionClose, SIGNAL(triggered()), this, SLOT(closeDevice()));
  connect(ui.tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));

  try
  {
    PythonCore::Instance()->setParent(this);
    PythonCore::Instance()->initPython();
  }
  catch (md_exception& e)
  {
    LOG_ERROR("PYTHON INIT FAILED: %s", e.what());
  }

  addDevice("ui/IOBoard_0.ui");
  
  SETTINGS->beginGroup("MainWindow");
  restoreGeometry(SETTINGS->value("geometry").toByteArray());
  restoreState(SETTINGS->value("state").toByteArray());
  auto files = SETTINGS->value("open_list").toStringList();
  ui.splitter->restoreState(SETTINGS->value("splitter").toByteArray());
  SETTINGS->endGroup();
  for each (auto f in files)
  {
    //addDevice(f);
  }
  
  
  
  
}

ModbusDevices::~ModbusDevices()
{
  SETTINGS->beginGroup("MainWindow");
  SETTINGS->setValue("geometry", saveGeometry());
  SETTINGS->setValue("state", saveState());
  SETTINGS->setValue("splitter", ui.splitter->saveState());
  SETTINGS->setValue("open_list", _opened_files);
  SETTINGS->sync();
  SETTINGS->endGroup();
}

void ModbusDevices::addDevice(const QString& ui_file)
{
  LOG_INFO("Add widget from file: %s", ui_file.toUtf8().constData());
  DeviceWidget* widget(new DeviceWidget(this));
  try
  {
    widget->load(ui_file);
    auto idx = ui.tabWidget->addTab(widget, widget->windowTitle());
    ui.tabWidget->setCurrentIndex(idx);
    _opened_files.append(ui_file);
    connect(widget, SIGNAL(started()), this, SLOT(closeTab(int)));
    //ui.tabWidget->tabBar()->setTabTextColor(idx, "red");
  }
  catch (std::runtime_error& ex)
  {
    delete widget;
    LOG_ERROR("EXCEPTION: %s", ex.what());
  }
  
}

void ModbusDevices::removeDeviceTab(int idx)
{
  DeviceWidget* w = qobject_cast<DeviceWidget*>(ui.tabWidget->widget(idx));
  if (w)
  {
    LOG_INFO("Remove widget %s", w->windowTitle().toLatin1().constData());
    if (w->inWork())
    {
      QMessageBox msgBox;
      msgBox.setIcon(QMessageBox::Warning);
      msgBox.setText("<b>Device is working now!</b>");
      msgBox.setInformativeText("Do you want to stop it anyway?");
      msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
      msgBox.setDefaultButton(QMessageBox::Ok);
      if (msgBox.exec() == QMessageBox::Cancel)
      {
        LOG_INFO("Canceled");
        return;
      }
    }
    if (w->stopPoll())
    {
      ui.tabWidget->removeTab(idx);
      _opened_files.removeAt(idx);
    }
      
  }
}

void ModbusDevices::openDevice()
{
  QString fname = QFileDialog::getOpenFileName(this, tr("Open device file"), QDir::currentPath(), "Qt Designer files (*.ui*);;All files (*.*)");
  if (!fname.isNull())
  {
    addDevice(fname);
  }
}

void ModbusDevices::closeDevice()
{
  removeDeviceTab(ui.tabWidget->currentIndex());
}

void ModbusDevices::closeTab(int tab)
{
  removeDeviceTab(tab);
}
