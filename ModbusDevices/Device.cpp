#include "Device.h"

Device::Device(QObject *parent, DeviceWidget* widget)
: QThread(parent), widget(widget), abort(false)
{

}

Device::~Device()
{
  switchOff();
}

void Device::run()
{
  while (!this->abort)
  {
    widget->poll();
    //for (auto &dev : this->devices)
    //{
    //  dev->poll();
    //}
    //sleep(1);
  }
}

void Device::switchOn()
{
  this->abort = false;
  this->start();
}

void Device::switchOff()
{
  if (!this->isRunning())
    return;
  this->abort = true;
  this->wait();
}

