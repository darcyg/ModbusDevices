#include "DeviceWidget.h"
#include "ModbusDevices.h"
#include "Settings.hpp"
#include <QMessageBox>
#include <QFileDialog>

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

  addDevice("ui/Tura.ui");
  
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

void ModbusDevices::addDevice(const QString& ui_file)
{
  LOG_INFO("Add widget from file: %s", ui_file.toUtf8().constData());
  DeviceWidget* widget(new DeviceWidget(this));
  try
  {
    widget->load(ui_file);
    ui.tabWidget->addTab(widget, widget->windowTitle());
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
    if(w->stopPoll())
      ui.tabWidget->removeTab(idx);
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
