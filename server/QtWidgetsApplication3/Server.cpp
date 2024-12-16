#include "Server.h"
#include "ui_QtWidgetsApplication3.h"
#include <QHostAddress>
#include <iostream>

Server::Server(Ui::QtWidgetsApplication3* ui, QWidget* parent, std::string port)
    : QWidget(parent), isRunning(false), ui(ui), IPport(port) {
    connect(this, &Server::logMessage, this, &Server::onLogMessage);

    udpSocket = new QUdpSocket(this);
    broadcastTimer = new QTimer(this);
    connect(broadcastTimer, &QTimer::timeout, this, &Server::handleBroadcast);

    startGrpcServer();
    startBroadcast();
}

Server::~Server() {
    stopGrpcServer();

    if (udpSocket) delete udpSocket;
    if (broadcastTimer) delete broadcastTimer;
}

void Server::startGrpcServer() {
    isRunning = true;

    grpcThread = std::thread([this]() {
        try {
            std::string serverAddress("0.0.0.0:" + IPport);
            ServerServiceImpl service;

            service.setLogger([this](const std::string& message) {
                emit logMessage(QString::fromStdString(message)); 
                });

            grpc::ServerBuilder builder;
            builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
            builder.RegisterService(&service);

            grpcServer = builder.BuildAndStart();
            emit logMessage("gRPC server listening on " + QString::fromStdString(serverAddress));

            grpcServer->Wait();
            emit logMessage("gRPC server stopped.");
        }
        catch (const std::exception& e) {
            emit logMessage("gRPC Server Error: " + QString::fromStdString(e.what()));
        }
        });
}

void Server::stopGrpcServer() {
    if (grpcServer) {
        grpcServer->Shutdown();
        isRunning = false;
    }

    if (grpcThread.joinable()) {
        grpcThread.join();
    }
}

void Server::startBroadcast() {
    broadcastTimer->start(5000);
}

void Server::handleBroadcast() {
    QString message = "Server is running on port 10001";
    udpSocket->writeDatagram(message.toUtf8(), QHostAddress::Broadcast, 10001);
    emit logMessage("Broadcast message sent.");
}

void Server::onLogMessage(const QString& message) {
    ui->textEdit->append(message);
}
