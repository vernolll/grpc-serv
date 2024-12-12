#include "MainWindow.h"
#include <QDateTime>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), client(new Client(this))
{
    ui.setupUi(this);
    setupTable();

    connect(client, &Client::pingReceived, this, &MainWindow::updateLastPingTime);
    connect(client, &Client::connectionLost, this, &MainWindow::handleConnectionLost);
    connect(client, &Client::serverDiscovered, this, &MainWindow::addServer);
}

MainWindow::~MainWindow()
{
    delete client;
    // Удаляем кнопки и таймеры для каждого сервера
    for (auto& server : servers) {
        delete server.actionButton;
        delete server.pingTimer;
    }
}

void MainWindow::setupTable()
{
    ui.serverTable->setColumnCount(4);
    ui.serverTable->setHorizontalHeaderLabels({ "IP", "Last Ping Time", "Status", "Action" });
    ui.serverTable->horizontalHeader()->setStretchLastSection(true);
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

    // Обработчик кнопки
    connect(serverInfo.actionButton, &QPushButton::clicked, [this, ipPort]() {
        if (servers.contains(ipPort)) {
            ServerInfo& server = servers[ipPort];
            if (server.status == "Offline") {
                handleConnectClicked(server);  // Передаем ссылку на объект
            }
            else {
                handleDisconnectClicked(server);  // Передаем ссылку на объект
            }
        }
        });

    servers[ipPort] = serverInfo;
    addRowToTable(serverInfo);
}

void MainWindow::addRowToTable(const ServerInfo& serverInfo)
{
    int row = ui.serverTable->rowCount();
    ui.serverTable->insertRow(row);

    ui.serverTable->setItem(row, 0, new QTableWidgetItem(serverInfo.ipPort));
    ui.serverTable->setItem(row, 1, new QTableWidgetItem(serverInfo.lastPingTime));
    ui.serverTable->setItem(row, 2, new QTableWidgetItem(serverInfo.status));
    ui.serverTable->setCellWidget(row, 3, serverInfo.actionButton);
}

void MainWindow::handleConnectClicked(ServerInfo& server)
{
    server.status = "Online";
    server.actionButton->setText("Disconnect");

    // Начинаем пинговать сервер
    client->startPinging(server.ipPort.split(":").first(), server.ipPort.split(":").last().toInt());

    connect(server.pingTimer, &QTimer::timeout, [this, &server]() {
        server.missedPings++;  // Увеличиваем количество пропущенных пингов
        if (server.missedPings > 3) {
            handleConnectionLost(server.ipPort);
        }
        });

    server.pingTimer->start(5000);
}

void MainWindow::handleDisconnectClicked(ServerInfo& server)
{
    server.status = "Offline";
    server.actionButton->setText("Connect");

    // Останавливаем пинг и таймер
    client->stopPinging();
    server.pingTimer->stop();
    server.missedPings = 0;

    // Обновление таблицы
    for (int row = 0; row < ui.serverTable->rowCount(); ++row) {
        if (ui.serverTable->item(row, 0)->text() == server.ipPort) {
            ui.serverTable->item(row, 2)->setText(server.status);  // Устанавливаем статус как "Offline"
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

    // Обновление таблицы
    for (int row = 0; row < ui.serverTable->rowCount(); ++row) {
        if (ui.serverTable->item(row, 0)->text() == ipPort) {
            ui.serverTable->item(row, 1)->setText(server.lastPingTime);
            ui.serverTable->item(row, 2)->setText("Online");
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

    // Обновление таблицы
    for (int row = 0; row < ui.serverTable->rowCount(); ++row) {
        if (ui.serverTable->item(row, 0)->text() == ipPort) {
            ui.serverTable->item(row, 2)->setText("Offline");
            break;
        }
    }
}
