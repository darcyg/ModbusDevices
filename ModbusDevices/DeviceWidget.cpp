#include "DeviceWidget.h"
#include <QtUiTools>
#include <string>

DeviceWidget::DeviceWidget(QWidget *parent)
: QWidget(parent)
{
  ui.setupUi(this);
}

DeviceWidget::~DeviceWidget()
{
}

void DeviceWidget::load(const QString& ui_file)
{
  QUiLoader loader;

  QFile file(ui_file);
  if (!file.open(QFile::ReadOnly))
    throw std::exception("Can not open file");
  this->widget.reset(loader.load(&file));
  file.close();
  this->setWindowTitle(this->widget->windowTitle());
  ui.scrollArea->setWidget(this->widget.get());

  auto list = this->widget->findChildren<QWidget*>();
  foreach(QWidget *w, list)
  {
    QString str = w->styleSheet();
    if (str.length())
    {
      LOG_INFO(QString("Parse reg widget: ") + w->objectName());
      Register reg;
      if (!reg.loadJSON(std::string("{") + str.toUtf8().constData() + "}"))
        continue;
      LOG_SUCCESS("func: %d, addr: %d, fmt: %s", reg.function(), reg.address(), reg.format().c_str());
      this->registers.push_back(reg);
    }
  }


}