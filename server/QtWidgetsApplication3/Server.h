#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QUdpSocket>
#include <QTimer>
#include <QMutex>
#include <memory>
#include <thread>
#include <grpcpp/grpcpp.h>
#include "generated/api.pb.h"
#include "generated/api.grpc.pb.h"
#include "ui_QtWidgetsApplication3.h"
#include "ServerServiceImpl.h"

namespace Ui {
    class QtWidgetsApplication3;
}

class Server : public QWidget {
    Q_OBJECT

public:
    explicit Server(Ui::QtWidgetsApplication3* ui, QWidget* parent = nullptr, std::string port = "10001");
    ~Server();

private:
    void startGrpcServer();
    void stopGrpcServer();

    std::shared_ptr<grpc::Server> grpcServer;
    std::thread grpcThread;
    std::atomic<bool> isRunning;
    std::string IPport;

    QTimer* broadcastTimer;
    QUdpSocket* udpSocket;

    void startBroadcast();
    void handleBroadcast();

    Ui::QtWidgetsApplication3* ui;

signals:
    void logMessage(const QString& message);

private slots:
    void onLogMessage(const QString& message);
};

#endif // SERVER_H
