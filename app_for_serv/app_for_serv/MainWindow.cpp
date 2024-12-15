#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), client(new Client(this))
{
    ui->setupUi(this);
    setupTable();

    // Соединение сигналов с соответствующими слотами
    connect(client, &Client::pingReceived, this, &MainWindow::updateLastPingTime);
    connect(client, &Client::connectionLost, this, &MainWindow::handleConnectionLost);

    // Подключаемся к серверу при запуске
    connectToServer("127.0.0.1:10001");  // Замените на реальный адрес и порт
}

MainWindow::~MainWindow()
{
    delete ui;
    delete client;
    for (auto& server : servers) {
        delete server.actionButton;
        delete server.pingTimer;
    }
}

void MainWindow::setupTable()
{
    ui->serverTable->setColumnCount(4);
    ui->serverTable->setHorizontalHeaderLabels({ "IP", "Last Ping Time", "Status", "Action" });
    ui->serverTable->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::addServer(const QString& ipPort)
{
    if (servers.contains(ipPort)) {
        qDebug() << "Server already added:" << ipPort;
        return;
    }

    ServerInfo serverInfo;
    serverInfo.ipPort = ipPort;
    serverInfo.lastPingTime = "-";
    serverInfo.status = "Offline";
    serverInfo.actionButton = new QPushButton("Connect");
    serverInfo.pingTimer = new QTimer(this);
    serverInfo.missedPings = 0;

    // При нажатии на кнопку "Connect" или "Disconnect"
    connect(serverInfo.actionButton, &QPushButton::clicked, [this, ipPort]() {
        if (servers.contains(ipPort)) {
            ServerInfo& server = servers[ipPort];
            if (server.status == "Offline") {
                handleConnectClicked(server);
            }
            else {
                handleDisconnectClicked(server);
            }
        }
        });

    servers[ipPort] = serverInfo;
    addRowToTable(serverInfo);
}

void MainWindow::addRowToTable(const ServerInfo& serverInfo)
{
    int row = ui->serverTable->rowCount();
    ui->serverTable->insertRow(row);

    ui->serverTable->setItem(row, 0, new QTableWidgetItem(serverInfo.ipPort));
    ui->serverTable->setItem(row, 1, new QTableWidgetItem(serverInfo.lastPingTime));
    ui->serverTable->setItem(row, 2, new QTableWidgetItem(serverInfo.status));
    ui->serverTable->setCellWidget(row, 3, serverInfo.actionButton);
}

void MainWindow::handleConnectClicked(ServerInfo& server)
{
    server.status = "Online";
    server.actionButton->setText("Disconnect");

    // Запуск таймера для отслеживания пропущенных пингов
    connect(server.pingTimer, &QTimer::timeout, [this, &server]() {
        server.missedPings++;
        if (server.missedPings > 3) {
            handleConnectionLost(server.ipPort);
        }
        });

    server.pingTimer->start(5000);  // Пинг каждые 5 секунд
}


void MainWindow::handleDisconnectClicked(ServerInfo& server)
{
    // Обновляем статус на "Offline"
    server.status = "Offline";
    server.actionButton->setText("Connect");

    // Останавливаем пинг для конкретного сервера
    client->stopPinging();  // Здесь нужно будет убедиться, что клиент может работать с несколькими серверами (если потребуется)

    // Останавливаем таймер для данного сервера
    if (server.pingTimer) {
        server.pingTimer->stop();
    }

    // Сброс пропущенных пингов
    server.missedPings = 0;

    // Обновляем статус в таблице для данного сервера
    for (int row = 0; row < ui->serverTable->rowCount(); ++row) {
        if (ui->serverTable->item(row, 0)->text() == server.ipPort) {
            ui->serverTable->item(row, 2)->setText(server.status);  // Обновляем статус на "Offline"
            break;
        }
    }
}


void MainWindow::updateLastPingTime(const QString& ipPort)
{
    if (!servers.contains(ipPort)) {
        return;
    }

    ServerInfo& server = servers[ipPort];
    server.lastPingTime = QDateTime::currentDateTime().toString("hh:mm:ss");
    server.missedPings = 0;

    for (int row = 0; row < ui->serverTable->rowCount(); ++row) {
        if (ui->serverTable->item(row, 0)->text() == ipPort) {
            ui->serverTable->item(row, 1)->setText(server.lastPingTime);
            ui->serverTable->item(row, 2)->setText("Online");
            break;
        }
    }
}

void MainWindow::handleConnectionLost(const QString& ipPort)
{
    if (!servers.contains(ipPort)) {
        return;
    }

    ServerInfo& server = servers[ipPort];
    server.status = "Offline";
    server.actionButton->setText("Connect");
    server.pingTimer->stop();

    for (int row = 0; row < ui->serverTable->rowCount(); ++row) {
        if (ui->serverTable->item(row, 0)->text() == ipPort) {
            ui->serverTable->item(row, 2)->setText("Offline");
            break;
        }
    }
}

void MainWindow::connectToServer(const QString& ipPort)
{
    addServer(ipPort);
    ServerInfo& server = servers[ipPort];

    // Разделяем IP и порт
    QStringList ipPortList = ipPort.split(":");
    QString serverIp = ipPortList.first();
    int serverPort = ipPortList.last().toInt();

    // Подключаемся к серверу
    client->connectToServer(serverIp, serverPort);  
}
