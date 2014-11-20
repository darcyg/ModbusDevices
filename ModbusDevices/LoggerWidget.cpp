
#include "LoggerWidget.h"
#include "stdarg.h"
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

void LoggerWidget::print( Level lev, QString& msg, ... )
{
  QTime time = QTime::currentTime();
  emit LoggerWidget::instance->append( time.toString("hh:mm:ss.zzz") + "\t" + msg );
}

LoggerWidget::LoggerWidget(QWidget *parent) : QTextEdit(parent)
{
  if(instance)
    delete instance;
  connect( this, SIGNAL(print_signal(int, QString)), this, SLOT(print_slot(int, QString)), Qt::QueuedConnection );
  instance = this;
}

LoggerWidget::~LoggerWidget()
{
}

void LoggerWidget::print_slot(int level, QString msg)
{
  this->append(QString(msg));
}

