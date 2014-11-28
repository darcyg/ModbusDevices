#include "Convert.h"
#include <QLineEdit>
#include <QCheckBox>


void valueToWidget(const Value &v, QWidget* w)
{
  if (auto obj = qobject_cast<QLineEdit*>(w))
  {
    float val = v.toFloat();
    obj->setText(QString::number(val));
  }
  else if (auto obj = qobject_cast<QCheckBox*>(w))
  {
    obj->setChecked(v.v_u32);
  }
}

void widgetToValue(QWidget* w, Value &v)
{
  if (auto obj = qobject_cast<QLineEdit*>(w))
  {
    v = obj->text().toFloat();
  }
  else if (auto obj = qobject_cast<QCheckBox*>(w))
  {
    v = obj->isChecked() ? 1 : 0;
  }
}