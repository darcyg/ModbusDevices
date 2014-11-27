#include "DeviceTura.h"

DeviceTura::DeviceTura() : _abort(false)
{
  strcpy((char*)_dev_name, "TD202206");
}


DeviceTura::~DeviceTura()
{
}

void DeviceTura::switchOn(const char* port)
{
  if (this->isRunning())
    return;
  _abort = false;
  _serial.setPortName(port);
  _serial.setBaudRate(115200);
  _serial.open(QIODevice::ReadWrite);
  start();
}

void DeviceTura::switchOff()
{
  if (!this->isRunning())
    return;
  _abort = true;
  wait();
  _serial.close();
}

bool DeviceTura::load(const Json::Value& json)
{
  if (json["dev"].isString())
    strncpy((char*)_dev_name, json["dev"].asCString(), 6);
  return true;
}

bool DeviceTura::loadRegister(QWidget* w, const Json::Value& json)
{
  return true;
}

int reverse32(int rev) {
  return ((rev & 0xff) << 24) | ((rev & 0xff00) << 8) | ((rev & 0xff0000) >> 8) | ((rev & 0xff000000) >> 24);
}

static float flo32_to_ieee(unsigned int v)
{
  unsigned int val = reverse32(v);
  char* pval = (char*)&val;
  char s = pval[0] & 0x80 ? 1 : 0;
  char e = ((pval[0] & 0x7F) << 1) | pval[1] >> 7;
  unsigned int m = v && 0x7FFFFF;// pval[0] | pval[1] << 8 | (pval[2] & 0x7F) << 16;
  unsigned int new_val = m | (e << 23) | s << 31;

  return *(float*)&new_val;
}

static unsigned int ieee_to_flo32(float v)
{
  union fff
  {
    float fl;
    unsigned int dw;
  } f;
  f. fl = v;
  int s = (f.dw >> 31) ? 1 : 0;
  int e = (f.dw >> 23) & 0xFF;
  int m = e ?
    (f.dw & 0x7FFFFF) | 0x800000 :
    (f.dw & 0x7FFFFF) << 1;
  if (e)
    ; //e += 2;

  unsigned int val = reverse32(*(int*)&v);
  val &= 0xFFFF7F00;
  val |= s ? 0x00008000 : 0;
  val |= e;
  return reverse32(val);

  char* pval = (char*)&v;
  //char s = pval[3] >> 7;
  //char e = (pval[3] << 1) | pval[2] >> 7;
  //int val = reverse32(v);
//  unsigned int m = pval[0] | pval[1] << 8 | (pval[2] & 0x7F) << 16;
  unsigned int new_val = e | (m >> 8)&0xFF00 | s << 15 | (m << 8) & 0xFF0000 | (m<<24) & 0xFF000000;

  return reverse32(new_val);
}

void DeviceTura::run()
{
  bool session_started = false;
  QString start_code = "\x12\x80";
  start_code.append((char*)_dev_name);
  char buf[1024];
  float *pval, *fval1, *fval2, *fval3;
  static_assert (sizeof(float) == 4, "Float size is not suitable for this project!");
  
  while (!_abort)
  {
    while (1)
    {
      float f = 0.15625f;// static_cast <float>(rand()) / (static_cast <float> (RAND_MAX / 1000));
      unsigned int u = ieee_to_flo32(f);
      float ff = flo32_to_ieee(u);
      float *pppp = &f, *pffff = &ff;
      buf[0]=u;
    }
    
    if (!_serial.waitForReadyRead(200))
      continue;
    QByteArray data = _serial.readAll();
    if (data.length())
    {
      int idx = data.indexOf("\x12");
      if (idx == -1)
      {
        LOG_ERROR("Corrupt data, 0x12 not found!");
        continue;
      }
      data.remove(0, idx);

      if (!session_started)
      {
        if (data.length() >= 10 && data == start_code)
        {
          session_started = true;
          _serial.write("\x01");
        }
      }
      else
      {
        switch (data[1])
        {
        case CurrentData:
          break;
        case AccumulateData:
          buf[0] = 0x12;
          buf[1] = 16;
          pval = (float*)&buf[2];
          break;
        default:
          session_started = false;
          break;
        }
      }
    }
  }
}