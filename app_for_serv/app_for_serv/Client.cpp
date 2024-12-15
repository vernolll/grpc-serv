#include "Client.h"
#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <grpcpp/grpcpp.h>
#include "generated/api.pb.h"
#include "generated/api.grpc.pb.h"

Client::Client(QObject* parent)
    : QObject(parent),
    udpSocket(new QUdpSocket(this)),
    grpcChannel(nullptr),
    grpcStub(nullptr),
    pingTimer(new QTimer(this))
{
    connect(udpSocket, &QUdpSocket::readyRead, this, &Client::processBroadcast);
    connect(pingTimer, &QTimer::timeout, this, &Client::sendPing);

    // Биндим UDP сокет для прослушивания широковещательных сообщений
    if (!udpSocket->bind(10000, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        qDebug() << "Failed to bind UDP socket to port 10000.";
        QMessageBox::critical(nullptr, "Error", "Failed to bind UDP socket to port 10000.");
    }
    else {
        qDebug() << "Listening for server broadcasts on port 10000.";
    }
}

Client::~Client() {
    delete udpSocket;
    delete pingTimer;
}

void Client::processBroadcast() {
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());

        QString serverInfo = QString::fromUtf8(datagram);
        qDebug() << "Received broadcast from server:" << serverInfo;

        QStringList parts = serverInfo.split(':');
        if (parts.size() == 2) {
            QString serverIp = parts[0];
            int serverPort = parts[1].toInt();

            connectToServer(serverIp, serverPort);
            break;  // После первого успешного подключения прекращаем обработку
        }
    }
}

void Client::connectToServer(const QString& serverIp, int serverPort) {
    QString serverAddress = serverIp + ":" + QString::number(serverPort);
    qDebug() << "Attempting to connect to gRPC server at:" << serverAddress;

    grpcChannel = grpc::CreateChannel(serverAddress.toStdString(), grpc::InsecureChannelCredentials());
    grpcStub = std::make_unique<MaintainingApi::Stub>(grpcChannel);

    // Проверяем подключение к серверу с использованием gRPC
    PingRequest request;
    PingResponse response;
    grpc::ClientContext context;

    request.set_clientip(QHostAddress(QHostAddress::LocalHost).toString().toStdString());

    grpc::Status status = grpcStub->Ping(&context, request, &response);

    if (status.ok()) {
        qDebug() << "Successfully connected to gRPC server.";
        QMessageBox::information(nullptr, "Connection", "Connected to server at " + serverAddress);
        startPinging();
    }
    else {
        qDebug() << "Failed to connect to gRPC server: " << QString::fromStdString(status.error_message());
        QMessageBox::critical(nullptr, "Connection Error", "Failed to connect to server.\nError: " + QString::fromStdString(status.error_message()));
    }
}

void Client::startPinging() {
    qDebug() << "Starting gRPC pinging...";
    pingTimer->start(5000);  // Пинг каждые 5 секунд
}

void Client::stopPinging() {
    qDebug() << "Stopping gRPC pinging...";
    pingTimer->stop();
}

void Client::sendPing() {
    PingRequest request;
    request.set_clientip(QHostAddress(QHostAddress::LocalHost).toString().toStdString());

    PingResponse response;
    grpc::ClientContext context;
    grpc::Status status = grpcStub->Ping(&context, request, &response);

    if (status.ok()) {
        emit pingReceived("127.0.0.1:10001");  // Замените на реальный IP и порт
    }
    else {
        qDebug() << "Ping failed: " << QString::fromStdString(status.error_message());
        emit connectionLost("127.0.0.1:10001");  // Замените на реальный IP и порт
    }
}
