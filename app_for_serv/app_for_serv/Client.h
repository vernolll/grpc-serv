#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "generated/api.pb.h"
#include "generated/api.grpc.pb.h"

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QObject* parent = nullptr);
    ~Client();

    // Методы для управления соединением
    void connectToServer(const QString& serverIp, int serverPort);
    void startPinging();
    void stopPinging();

signals:
    void pingReceived(const QString& serverIp);  // Сигнал, когда пинг получен от сервера
    void connectionLost(const QString& serverIp);  // Сигнал, когда связь с сервером потеряна


private slots:
    void processBroadcast();  // Обработка UDP широковещательных сообщений
    void sendPing();          // Отправка gRPC пинга

private:
    // UDP для получения широковещательных сообщений
    QUdpSocket* udpSocket;

    // gRPC подключение и Stub
    std::shared_ptr<grpc::Channel> grpcChannel;
    std::unique_ptr<MaintainingApi::Stub> grpcStub;

    // Таймер для периодического отправления пингов
    QTimer* pingTimer;

    // Данные сервера
    QString currentServerIp;
    int currentServerPort;
};

#endif // CLIENT_H
