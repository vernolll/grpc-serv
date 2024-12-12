#pragma once

#include <QObject>
#include <QUdpSocket>
#include <QTimer>

class Client : public QObject
{
    Q_OBJECT

public:
    Client(QObject* parent = nullptr);
    ~Client();

    void startPinging(const QString& serverIp, int serverPort);
    void stopPinging();

signals:
    void pingReceived(const QString& ipPort);
    void connectionLost(const QString& ipPort);
    void serverDiscovered(const QString& ipPort);

private slots:
    void sendPing();
    void processResponse();
    void processBroadcast();

private:
    QUdpSocket* udpSocket;
    QTimer* pingTimer;
    QString currentServerIp;
    int currentServerPort;
    int missedPings;
};
