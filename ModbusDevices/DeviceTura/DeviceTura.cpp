#include "Convert.h"
#include <QTime>
#include "DeviceTura.h"
#include "Settings.hpp"

DeviceTura::DeviceTura() : _abort(false), _info_widget(nullptr)
{
  strcpy((char*)_dev_name, "TD202206");
  memset(&_io_stat, 0, sizeof(_io_stat));
}


DeviceTura::~DeviceTura()
{
  switchOff();
}

void DeviceTura::switchOn(const char* port, int baudrate)
{
  if (this->isRunning())
    return;
  _abort = false;
  _portName = port;
  start();
}

void DeviceTura::switchOff()
{
  if (!this->isRunning())
    return;
  _abort = true;
  wait();
}

bool DeviceTura::load(const Json::Value& json)
{
  if (json["dev"].isString())
    strncpy((char*)_dev_name, json["dev"].asCString(), 6);
  return true;
}

bool DeviceTura::loadRegister(QWidget* w, const Json::Value& json)
{
  if (!json["key"].isString() )
  {
    LOG_ERROR("'key' not found in Tura register config");
    return false;
  }

  auto key = json["key"].asString();
  if (key == "display_info")
  {
    _info_widget = qobject_cast<QLabel*>(w);
    updateDisplaInfo();
    return true;
  }

  if (!json["idx"].isIntegral() )
  {
    LOG_ERROR("'idx' not found in Tura register config");
    return false;
  }
  auto idx = json["idx"].asUInt();

  if (idx > 2)
  {
    LOG_ERROR("'idx' is out of range: %d", idx);
    return false;
  }

  RegisterTura *preg = nullptr;

  if (key == "current_flowrate")
  {
    preg = &_current_fr[idx];
  }
  else if (key == "accum_volume")
  {
    preg = &_accum_fr[idx];
  }
  else
    return false;

  preg->widget = w;

  return true;
}

int reverse32(int rev) {
  return ((rev & 0xff) << 24) | ((rev & 0xff00) << 8) | ((rev & 0xff0000) >> 8) | ((rev & 0xff000000) >> 24);
}

static float flo32_to_ieee(unsigned int v)
{
  unsigned int val = reverse32(v);
  char* pval = (char*)&val;
  char s = pval[2] >> 7;
  char e = pval[3] ? pval[3] : 0;
  unsigned int m = pval[0] | pval[1] << 8 | (pval[2] & 0x7F) << 16;
  unsigned int new_val = m | (e << 23) | s << 31;

  return *(float*)&new_val;
}

static unsigned int ieee_to_flo32(float v)
{
  unsigned int val = *(unsigned int*)&v;
  int s = (val >> 31) ? 1 : 0;
  int e = (val >> 23) & 0xFF;
  int m = e ? (val & 0x7FFFFF) | 0x800000 : (val & 0x7FFFFF) << 1;
  
  val = reverse32(val);
  val &= 0xFFFF7F00;
  val |= s ? 0x00008000 : 0;
  val |= e;
  return val;
}

int DeviceTura::serialRead(char *pbuf, int sz, unsigned int tm)
{
  QTime timer;
  timer.start();
  
  int i;
  DWORD rsz;
  LONG lLastError;

  for (i = 0; i < sz && timer.elapsed() < tm;)
  {
    lLastError = _cserial->Read(pbuf + i, sz - i, &rsz, 0, tm - timer.elapsed());
    if (lLastError != ERROR_SUCCESS)
      break;
    i += rsz;
  }
  _io_stat.rx_bytes += i;
  return i;
}

int DeviceTura::serialWrite(char *pbuf, int sz)
{
  DWORD wsz;
  LONG lLastError = _cserial->Write(pbuf, sz, &wsz, 0, 100);
  _io_stat.tx_bytes += wsz;
  return wsz;
}

void DeviceTura::processSession()
{
  char buf[128];
  float *pval, fval, fract;
  unsigned int *pu32, val;

  while (!_abort)
  {
    int rsz = serialRead(buf, 1, 100);
    if (rsz && buf[0] == 0x12 && serialRead(buf, 1, 100))
    {
      switch (buf[0])
      {
      case CurrentData:
        //LOG_INFO("CurrentData");
        widgetToValue(_current_fr[0].widget, _current_fr[0].value);
        widgetToValue(_current_fr[1].widget, _current_fr[1].value);
        widgetToValue(_current_fr[2].widget, _current_fr[2].value);
        buf[0] = 1;
        buf[1] = 16;
        pu32 = (unsigned*)&buf[2];
        *pu32 = ieee_to_flo32(_current_fr[0].value.v_float);
        pu32++;
        *pu32 = ieee_to_flo32(_current_fr[1].value.v_float);
        pu32++;
        *pu32 = ieee_to_flo32(_current_fr[2].value.v_float);
        serialWrite(buf, 18);
        if (!serialRead(buf, 1, 100) || buf[0] != 1)
          return;
        break;
      case AccumulateData:
        //LOG_INFO("AccumulateData");
        widgetToValue(_accum_fr[0].widget, _accum_fr[0].value);
        widgetToValue(_accum_fr[1].widget, _accum_fr[1].value);
        widgetToValue(_accum_fr[2].widget, _accum_fr[2].value);
        memset(buf, 0, 66);
        buf[0] = 1;
        buf[1] = 64;
        
        pu32 = (unsigned int*)&buf[10];
        *pu32 = ieee_to_flo32(modf(_accum_fr[0].value.v_float, &fval));
        val = fval;
        buf[26] = val >> 16;
        buf[27] = val >> 8;
        buf[28] = val >> 0;

        pu32 = (unsigned int*)&buf[14];
        *pu32 = ieee_to_flo32(modf(_accum_fr[1].value.v_float, &fval));
        val = fval;
        buf[29] = val >> 16;
        buf[30] = val >> 8;
        buf[31] = val >> 0;

        pu32 = (unsigned int*)&buf[18];
        *pu32 = ieee_to_flo32(modf(_accum_fr[2].value.v_float, &fval));
        val = fval;
        buf[32] = val >> 16;
        buf[33] = val >> 8;
        buf[34] = val >> 0;

        serialWrite(buf, 66);
        if (!serialRead(buf, 1, 100) || buf[0] != 1)
          return;
        break;
      default:
        LOG_ERROR("UNKNOWN COMMAND: %d", buf[0]);
        return;
      }
    }

  }
}


void DeviceTura::run()
{
  char buf[1024];
  float *pval;
  unsigned int *pu32;
  static_assert (sizeof(float) == 4, "Float size is not suitable for this project!");

  _cserial = new CSerial();
  _cserial->Setup((CSerial::EBaudrate)115200, CSerial::EData8, CSerial::EParNone, CSerial::EStop1);
  LONG lLastError = _cserial->Open((LPCTSTR)_portName.toLatin1().constData(), 4096, 4096, true);
  if (lLastError != ERROR_SUCCESS)
  {
    LOG_ERROR("Open port failed: %d", lLastError);
    return;
  }

  while (!_abort)
  {
    int rsz = serialRead(buf, 1, 1000);
    if (rsz && buf[0] == 0x12)
    {
      if (serialRead(buf, 1, 100))
      {
        if (serialRead(buf, 8, 100) == 8 && !memcmp(buf, _dev_name, 8))
        {
          static DWORD last_tick = GetTickCount();
          serialWrite("\x01", 1);
          processSession();
          if ((GetTickCount() - last_tick) >= 800)
          {
            updateDisplaInfo();
            last_tick = GetTickCount();
          }
          _io_stat.sessions++;
          //LOG_INFO("Session stopped");
        }
      }
    }
  }
  LOG_INFO("Tura thread finish");
  _cserial->Close();
  delete _cserial;
}
#include <QTextStream>

void DeviceTura::updateDisplaInfo()
{
  QString str;
  QTextStream(&str) << "Tx/Rx:\t" << _io_stat.tx_bytes << "/" << _io_stat.rx_bytes << "\nSessions:\t" << _io_stat.sessions;
  _info_widget->setText(str);
}

void DeviceTura::saveState(QString &key)
{
  for (auto &v : _current_fr)
    widgetToValue(v.widget, v.value);
  for (auto &v : _accum_fr)
    widgetToValue(v.widget, v.value);
  SETTINGS->beginGroup(key);
  SETTINGS->setValue("fr0", _current_fr[0].value.v_float);
  SETTINGS->setValue("fr1", _current_fr[1].value.v_float);
  SETTINGS->setValue("fr2", _current_fr[2].value.v_float);
  SETTINGS->setValue("av0", _accum_fr[0].value.v_float);
  SETTINGS->setValue("av1", _accum_fr[1].value.v_float);
  SETTINGS->setValue("av2", _accum_fr[2].value.v_float);
  SETTINGS->sync();
  SETTINGS->endGroup();
}

void DeviceTura::loadState(QString &key)
{
  SETTINGS->beginGroup(key);
  _current_fr[0].value.v_float = SETTINGS->value("fr0").toFloat();
  _current_fr[1].value.v_float = SETTINGS->value("fr1").toFloat();
  _current_fr[2].value.v_float = SETTINGS->value("fr2").toFloat();
  _accum_fr[0].value.v_float = SETTINGS->value("av0").toFloat();
  _accum_fr[1].value.v_float = SETTINGS->value("av1").toFloat();
  _accum_fr[2].value.v_float = SETTINGS->value("av2").toFloat();
  SETTINGS->endGroup();
  for (auto &v : _current_fr)
    valueToWidget(v.value, v.widget);
  for (auto &v : _accum_fr)
    valueToWidget(v.value, v.widget);
}