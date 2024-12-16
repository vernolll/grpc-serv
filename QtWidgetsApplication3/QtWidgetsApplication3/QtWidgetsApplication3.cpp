#include "QtWidgetsApplication3.h"

QtWidgetsApplication3::QtWidgetsApplication3(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    connect(ui.pushButton, &QPushButton::clicked, this, &QtWidgetsApplication3::onStartButtonClicked);

    show();
}

QtWidgetsApplication3::~QtWidgetsApplication3()
{
    delete server;
}

void QtWidgetsApplication3::onStartButtonClicked() {
    bool ok;
    int serverPort = ui.lineEdit->text().toInt(&ok);

    if (!ok || serverPort <= 0) {
        ui.textEdit->append("Invalid port! Please enter a valid port.");
        return; 
    }

    if (server) {
        ui.textEdit->append("Old server disconnected.");
        delete server; 
    }

    server = new Server(&ui, this, std::to_string(serverPort));
    ui.textEdit->append("Server started at port " + QString::number(serverPort));
}
