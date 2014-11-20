#pragma once

#include <QSettings>

#define SETTINGS  Settings::Instance()

class Settings : QObject
{
  Q_OBJECT

protected:
  Settings(QObject *parent = NULL);

public:
  ~Settings();

  static QSettings *CreateInstance()
  {
    DestroyInstance();
    return Instance();
  }

  static void DestroyInstance()
  {
    delete m_Settings;
    m_Settings = NULL;
  }

  static QSettings *Instance()
  {
    if (m_Settings == NULL)
      m_Settings = new QSettings("config.ini", QSettings::IniFormat);
    return m_Settings;
  }
private:
  static QSettings *m_Settings;

};
