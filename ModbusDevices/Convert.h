#pragma once
#include <QWidget>
#include "Value.h"

void valueToWidget(const Value &v, QWidget* w);
void widgetToValue(QWidget* w, Value &v);
