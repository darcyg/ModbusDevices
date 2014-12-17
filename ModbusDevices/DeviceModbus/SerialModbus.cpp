#include "SerialModbus.h"
#include <QObject>
#include <QTimer>

SerialModbus::SerialModbus()
{
  
}


SerialModbus::~SerialModbus()
{
}

std::map<const char*, std::shared_ptr<SerialModbus>> SerialModbus::_ports;
SerialModbus* SerialModbus::_dummy = nullptr;

void SerialModbus::poll()
{
  
}

std::shared_ptr<SerialModbus> SerialModbus::open(const char* name, int baudrate, fn_dataReceived cb)
{
  if (!_dummy)
  {
    _dummy = new SerialModbus;
    QTimer* timer = new QTimer;
    _dummy->connect(timer, SIGNAL(timeout()), SLOT(poll()));
    timer->start(10);
  }
  

  auto port_it = _ports.find(name);
  if (port_it != _ports.end())
  {
    if (port_it->second->_baudrate != baudrate)
      MD_THROW("Serial port is open already but with different baudrate");
    port_it->second->_dataReceived.push_back(cb);
    return port_it->second;
  }
  
  std::shared_ptr<SerialModbus> port(new SerialModbus());
  //port->_name = name;
  //port->_baudrate = baudrate;
  //port->_port.setPortName(name);
  //port->_port.setBaudRate(baudrate);
  //if (!port->_port.open(QSerialPort::ReadWrite))
  //  MD_THROW(port->_port.errorString().toLatin1().constData());
  //port->_dataReceived.push_back(cb);

  //_ports[name] = port;
  
  return port;
}

void SerialModbus::close(std::shared_ptr<SerialModbus> port)
{

}

void SerialModbus::readData()
{
  //QByteArray data = _port.readAll();
  //for (auto cb : _dataReceived)
  //  cb(data.constData(), data.size());
}