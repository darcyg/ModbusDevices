#ifndef _LOGGER_WIDGET_H
#define _LOGGER_WIDGET_H

#include <QTextEdit>
#include "stdarg.h"
#include "stdarg.h"

class LoggerWidget : public QTextEdit
{
  Q_OBJECT

public:
  LoggerWidget(QWidget *parent = 0);
  ~LoggerWidget();

  enum Level
  {
    Fatal =  -2,
    Error =  -1,
    Info =    0,
    Success = 1,
    Debug   = 2
  };

  static void print(Level level, const char* msg, ...);
  static void print(Level level, const QString& msg);
  static void print(Level level, const std::string& msg);
  static void printv(Level level, const char *msg, va_list vlist);

signals:
  void print_signal( int level, QString msg );

public slots:
  void print_slot(int level, QString msg);

private:
  static void print_base(Level level, const QString& msg);
  static LoggerWidget *instance;
};

inline void LOG_INFO(const char* msg, ...)
{
  va_list vl;
  va_start(vl, msg);
  LoggerWidget::printv(LoggerWidget::Info, msg, vl);
  va_end(vl);
}

inline void LOG_INFO(const QString& msg)
{
  LoggerWidget::print(LoggerWidget::Info, msg);
}

inline void LOG_INFO(const std::string& msg)
{
  LoggerWidget::print(LoggerWidget::Info, msg);
}

inline void LOG_ERROR(const char* msg, ...)
{
  va_list vl;
  va_start(vl, msg);
  LoggerWidget::printv(LoggerWidget::Error, msg, vl);
  va_end(vl);
}

inline void LOG_ERROR(const QString& msg)
{
  LoggerWidget::print(LoggerWidget::Error, msg);
}

inline void LOG_ERROR(const std::string& msg)
{
  LoggerWidget::print(LoggerWidget::Error, msg);
}

inline void LOG_SUCCESS(const char* msg, ...)
{
  va_list vl;
  va_start(vl, msg);
  LoggerWidget::printv(LoggerWidget::Success, msg, vl);
  va_end(vl);
}

inline void LOG_SUCCESS(const QString& msg)
{
  LoggerWidget::print(LoggerWidget::Success, msg);
}

inline void LOG_SUCCESS(const std::string& msg)
{
  LoggerWidget::print(LoggerWidget::Success, msg);
}

#endif // QLOGGER_H

