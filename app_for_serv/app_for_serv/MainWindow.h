#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include "Client.h"
#include <QPushButton>
#include <QTableWidgetItem>
#include <QTimer>
#include <QMap>
#include "serverInfo.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void setupTable();
    void addServer(const QString& ipPort);
    void handleDisconnectClicked(ServerInfo& server);
    void handleConnectClicked(ServerInfo& server);
    void updateLastPingTime(const QString& ipPort);
    void handleConnectionLost(const QString& ipPort);

private:
    void addRowToTable(const ServerInfo& serverInfo);

    Ui::MainWindowClass ui;
    Client* client;
    QMap<QString, ServerInfo> servers;  // Для хранения данных о серверах
};
