#ifndef _LOGGER_WIDGET_H
#define _LOGGER_WIDGET_H

#include <QTextEdit>
#define LOG_INFO(msg,...)  LoggerWidget::print(LoggerWidget::Info, msg, __VA_ARGS__)
#define LOG_ERROR(msg,...)  LoggerWidget::print(LoggerWidget::Error, msg, __VA_ARGS__)

#define QLOG_INFO(msg,...)  LoggerWidget::print(LoggerWidget::Info, msg, __VA_ARGS__)

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

  //virtual void print( int level, const char* msg, ... );

  static void print( Level level, const char* msg, ... );
  static void print( Level level, QString& msg, ... );

signals:
  void print_signal( int level, QString msg );

public slots:
  void print_slot(int level, QString msg);

private:
  static LoggerWidget *instance;
};


#endif // QLOGGER_H

