#include "Server.h"
#include <QHostAddress>
#include <QTextEdit>
#include <iostream>
#include <grpcpp/grpcpp.h>
#include "generated/api.pb.h"
#include "generated/api.grpc.pb.h"

Server::Server(Ui::QtWidgetsApplication3* ui, QWidget* parent)
    : QWidget(parent), ui(ui), isClientConnected(false), grpcServer(nullptr)
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

    if (grpcServer) {
        grpcServer->Shutdown(); // ��������� gRPC �������
    }
}

void Server::onStartButtonClicked() {
    bool ok;
    serverPort = ui->lineEdit->text().toInt(&ok);
    if (!ok || serverPort <= 0) {
        ui->textEdit->append("Invalid port!");
        return;
    }

    serverIp = QHostAddress(QHostAddress::LocalHost).toString();
    setupGrpcServer(); // ������������� gRPC �������
    startBroadcast();  // �������� �������

    ui->label->setText("Server Status: Broadcasting...");
    ui->textEdit->append("Broadcasting server info...");
}

void Server::startBroadcast() {
    QString message = serverIp + ":" + QString::number(serverPort);
    qint64 bytesWritten = udpSocket->writeDatagram(message.toUtf8(), QHostAddress::Broadcast, 10001);  // ���������� �� ������������������ ������
    if (bytesWritten == -1) {
        ui->textEdit->append("Failed to send broadcast message.");
    }
    else {
        ui->textEdit->append("Broadcast message sent: " + message);
    }
    broadcastTimer->start();
}

void Server::stopBroadcast() {
    broadcastTimer->stop();
    ui->textEdit->append("Broadcast stopped.");
}

void Server::setupGrpcServer() {
    try {
        // ������� ������ �������
        auto service = std::make_shared<ServerServiceImpl>();

        // ������� ������
        grpc::ServerBuilder builder;
        std::string serverAddress = "0.0.0.0:" + std::to_string(serverPort); // ���������� 0.0.0.0 ��� ������������� ���� �����������
        builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
        builder.RegisterService(service.get());

        grpcServer = builder.BuildAndStart();

        if (grpcServer == nullptr) {
            ui->textEdit->append("Failed to start gRPC server. grpcServer is nullptr.");
            return;  // ������ �����, ���� ������ �� ������� �������.
        }

        ui->textEdit->append("gRPC server started at " + QString::fromStdString(serverAddress));

        // ������ gRPC ������� � ��������� ������
        std::thread grpcServerThread([this]() {
            if (grpcServer) {
                grpcServer->Wait(); // ������� �������� � ������ ������
            }
            else {
                ui->textEdit->append("gRPC server is nullptr when trying to wait.");
            }
            });
        grpcServerThread.detach(); // ����������� �����, ����� ������ ��������� �������� � ������� ������
    }
    catch (const std::exception& e) {
        ui->textEdit->append("Exception during server setup: " + QString::fromStdString(e.what()));
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
