// Server.h
#ifndef SERVER_H
#define SERVER_H

#include "ui_QtWidgetsApplication3.h"
#include "ServerServiceImpl.h"
#include <QWidget>
#include <QUdpSocket>
#include <QTimer>
#include <grpcpp/grpcpp.h>
#include "generated/api.pb.h"
#include "generated/api.grpc.pb.h"

class Server : public QWidget {
    Q_OBJECT

public:
    explicit Server(Ui::QtWidgetsApplication3* ui, QWidget* parent = nullptr);
    ~Server();

private slots:
    void onStartButtonClicked();
    void handlePingTimeout();

private:
    void startBroadcast();
    void stopBroadcast();
    void setupGrpcServer();
    void setClientConnected(bool connected);

    QUdpSocket* udpSocket;
    QTimer* broadcastTimer;

    Ui::QtWidgetsApplication3* ui;

    int serverPort;
    QString serverIp;
    bool isClientConnected = false;  // Флаг подключения клиента

    std::unique_ptr<grpc::Server> grpcServer;  // gRPC сервер
};

#endif // SERVER_H
