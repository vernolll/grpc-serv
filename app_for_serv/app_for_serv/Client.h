#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QObject* parent = nullptr);
    ~Client();

    void startPinging(const QString& serverIp, int serverPort);  // Запуск пинга
    void stopPinging();  // Остановка пинга

signals:
    void pingReceived(const QString& ipPort);  // Сигнал о получении пинга
    void connectionLost(const QString& ipPort);  // Сигнал о потере соединения
    void serverDiscovered(const QString& ipPort);  // Сигнал об обнаружении сервера

private slots:
    void sendPing();  // Отправка пинга серверу
    void processResponse();  // Обработка ответа от сервера

private:
    QUdpSocket* udpSocket;  // UDP сокет для связи с сервером
    QTimer* pingTimer;  // Таймер для отправки пинга
    int missedPings;  // Счетчик пропущенных пингов
    QString currentServerIp;  // IP текущего сервера
    int currentServerPort;  // Порт текущего сервера

    void processBroadcast();  // Обработка широковещательных пакетов
};

#endif // CLIENT_H
