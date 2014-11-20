
#include "LoggerWidget.h"
#include <QDateTime>

#define MAX_LOG_LENGTH    1024


LoggerWidget* LoggerWidget::instance = NULL;

void LoggerWidget::print( Level lev, const char* msg, ... )
{
  char dat[MAX_LOG_LENGTH];

  va_list vl;
  va_start(vl, msg);
  int len = vsprintf_s(dat, MAX_LOG_LENGTH, msg, vl);
  va_end(vl);

  QTime time = QTime::currentTime();
  emit LoggerWidget::instance->append( time.toString("hh:mm:ss.zzz") + "\t" + dat );
}

void LoggerWidget::printv(Level level, const char *msg, va_list vlist)
{
  char dat[MAX_LOG_LENGTH];
  vsprintf_s(dat, MAX_LOG_LENGTH, msg, vlist);
  
  QTime time = QTime::currentTime();
  print_base(level, time.toString("hh:mm:ss.zzz") + "\t" + dat);
}

void LoggerWidget::print(Level level, const QString& msg)
{
  QTime time = QTime::currentTime();
  print_base(level, time.toString("hh:mm:ss.zzz") + "\t" + msg);
}

void LoggerWidget::print(Level level, const std::string& msg)
{
  QTime time = QTime::currentTime();
  print_base(level, time.toString("hh:mm:ss.zzz") + "\t" + msg.c_str());
}

void LoggerWidget::print_base(Level level, const QString& msg)
{
  
  //emit LoggerWidget::instance->append(msg);
  emit LoggerWidget::instance->print_signal(level, msg);
}

LoggerWidget::LoggerWidget(QWidget *parent) : QTextEdit(parent)
{
  if(instance)
    delete instance;
  connect(this, SIGNAL(print_signal(int, QString)), this, SLOT(print_slot(int, QString)), Qt::AutoConnection);
  instance = this;
}

LoggerWidget::~LoggerWidget()
{
}

void LoggerWidget::print_slot(int level, QString msg)
{
  switch (level)
  {
  case Error:
    this->setTextColor(Qt::red);
    break;
  case Success:
    this->setTextColor(Qt::darkGreen);
    break;
  default:
    this->setTextColor(Qt::black);
    break;
  }
  this->append(QString(msg));
}

