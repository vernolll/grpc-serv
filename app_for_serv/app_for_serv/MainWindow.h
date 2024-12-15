#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTimer>
#include <QMap>
#include <QTableWidgetItem>
#include "Client.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
    Client* client;  // Указатель на клиентский объект

    struct ServerInfo {
        QString ipPort;      // IP:Port сервера
        QString lastPingTime; // Время последнего пинга
        QString status;      // Статус сервера
        QPushButton* actionButton; // Кнопка для подключения/отключения
        QTimer* pingTimer;   // Таймер для пингов
        int missedPings;     // Счетчик пропущенных пингов
    };
    QMap<QString, ServerInfo> servers;  // Карта для хранения информации о серверах

    void setupTable();  // Инициализация таблицы
    void addServer(const QString& ipPort);  // Добавить сервер в таблицу
    void addRowToTable(const ServerInfo& serverInfo);  // Добавить строку в таблицу
    void handleConnectClicked(ServerInfo& server);  // Обработчик нажатия кнопки Connect
    void handleDisconnectClicked(ServerInfo& server);  // Обработчик нажатия кнопки Disconnect
    void updateLastPingTime(const QString& ipPort);  // Обновить время последнего пинга
    void handleConnectionLost(const QString& ipPort);  // Обработать потерю соединения
    void connectToServer(const QString& ipPort);  // Метод для подключения к серверу при запуске
};

#endif // MAINWINDOW_H
