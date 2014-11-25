#include "Convert.h"
#include <QLineEdit>
#include <QCheckBox>


void valueToWidget(const Value &v, QWidget* w)
{

}

void widgetToValue(QWidget* w, Value &v)
{
  if (auto obj = qobject_cast<QLineEdit*>(w))
  {
    float val = obj->text().toFloat();
    v =obj->text().toFloat();
  }
  else if (auto obj = qobject_cast<QCheckBox*>(w))
  {
    v = obj->isChecked() ? 1 : 0;
  }
}