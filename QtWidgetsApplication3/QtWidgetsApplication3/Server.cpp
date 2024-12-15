#include "Server.h"
#include <QHostAddress>
#include <QTextEdit>
#include <iostream>
#include <grpcpp/grpcpp.h>
#include "generated/api.pb.h"
#include "generated/api.grpc.pb.h"

Server::Server(Ui::QtWidgetsApplication3* ui, QWidget* parent)
    : QWidget(parent), ui(ui), isClientConnected(false)
{
    connect(ui->pushButton, &QPushButton::clicked, this, &Server::onStartButtonClicked);

    udpSocket = new QUdpSocket(this);
    broadcastTimer = new QTimer(this);

    broadcastTimer->setInterval(5000); // �������� �������

    connect(broadcastTimer, &QTimer::timeout, this, &Server::handlePingTimeout);
}

Server::~Server() {
    stopBroadcast();
    if (udpSocket) delete udpSocket;
    if (broadcastTimer) delete broadcastTimer;
}

void Server::onStartButtonClicked() {
    bool ok;
    serverPort = ui->lineEdit->text().toInt(&ok);
    if (!ok || serverPort <= 0) {
        ui->textEdit->append("Invalid port!");
        return;
    }

    serverIp = QHostAddress(QHostAddress::LocalHost).toString();
    setupGrpcServer();
    startBroadcast();

    ui->label->setText("Server Status: Broadcasting...");
    ui->textEdit->append("Broadcasting server info...");
}

void Server::startBroadcast() {
    QString message = serverIp + ":" + QString::number(serverPort);
    udpSocket->writeDatagram(message.toUtf8(), QHostAddress::Broadcast, 10001);  // ���������� �� ������������������ ������
    broadcastTimer->start();
    ui->textEdit->append("Broadcast message sent: " + message);
}

void Server::stopBroadcast() {
    broadcastTimer->stop();
}

void Server::setupGrpcServer() {
    auto service = std::make_shared<ServerServiceImpl>();  // ������������� �������
    grpc::ServerBuilder builder;
    builder.AddListeningPort(serverIp.toStdString() + ":" + std::to_string(serverPort), grpc::InsecureServerCredentials());
    builder.RegisterService(service.get());
    grpcServer = builder.BuildAndStart();

    if (grpcServer) {
        ui->textEdit->append("gRPC server started at " + serverIp + ":" + QString::number(serverPort));
    }
    else {
        ui->textEdit->append("Failed to start gRPC server.");
    }
}

void Server::handlePingTimeout() {
    // ���������, ���������� �� �������
    if (!isClientConnected) {
        ui->textEdit->append("No clients connected. Continuing broadcast...");
        startBroadcast();
    }
    else {
        ui->textEdit->append("Client connected. Stopping broadcast...");
        stopBroadcast();
    }
}

// ����� ����� ��� ��������� ����� � ����������� �������
void Server::setClientConnected(bool connected) {
    isClientConnected = connected;
}
