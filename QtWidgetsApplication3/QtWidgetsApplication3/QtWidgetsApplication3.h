#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsApplication3.h"
#include "Server.h"

class QtWidgetsApplication3 : public QMainWindow
{
    Q_OBJECT

public:
    QtWidgetsApplication3(QWidget* parent = nullptr);
    ~QtWidgetsApplication3();

private:
    Ui::QtWidgetsApplication3 ui;
    Server* server;
};

