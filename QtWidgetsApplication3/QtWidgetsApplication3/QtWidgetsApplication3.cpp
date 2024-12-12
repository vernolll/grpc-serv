#include "QtWidgetsApplication3.h"

QtWidgetsApplication3::QtWidgetsApplication3(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    server = new Server(&ui, this);
    show();
}

QtWidgetsApplication3::~QtWidgetsApplication3()
{
    delete server;
}
