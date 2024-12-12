#include "Server.h"
#include <QHostAddress>
#include <QUdpSocket>
#include <QTimer>
#include <QTextEdit>

Server::Server(Ui::QtWidgetsApplication3* ui, QWidget* parent) : QWidget(parent), ui(ui) {
    // ���������� ������� � �����
    connect(ui->pushButton, &QPushButton::clicked, this, &Server::onStartButtonClicked);

    // ������� ����� ��� UDP
    udpSocket = new QUdpSocket(this);

    // ������� �������
    pingTimer = new QTimer(this);
    broadcastTimer = new QTimer(this);

    // ������������� ���������
    pingTimer->setInterval(5000);  // ���� ������ �������
    broadcastTimer->setInterval(15000);  // ������� ����� 15 ������

    // ���������� ������� � ������
    connect(pingTimer, &QTimer::timeout, this, &Server::handleClientPing);
    connect(broadcastTimer, &QTimer::timeout, this, &Server::handlePingTimeout);
}

Server::~Server() {
    delete udpSocket;
    delete pingTimer;
    delete broadcastTimer;
}

void Server::onStartButtonClicked() {
    bool ok;
    serverPort = ui->lineEdit->text().toInt(&ok);  // �������� ���� �� QLineEdit
    if (!ok || serverPort <= 0) {
        ui->textEdit->append("Invalid port!");  // ���� ���� ��������, ������� ������ � QTextEdit
        return;
    }

    serverIp = QHostAddress(QHostAddress::LocalHost).toString();  // �������� ��������� IP

    // ��������� broadcast
    startBroadcast();

    ui->label->setText("Server Status: Broadcasting...");
    ui->textEdit->append("Broadcasting server info...");

    // �������� ����������� �������
    isClientConnected = true;
    pingTimer->start();  // �������� ���������� ����� �������
}

void Server::startBroadcast() {
    QString message = serverIp + ":" + QString::number(serverPort);

    // ���������� ��������� �� UDP
    udpSocket->writeDatagram(message.toUtf8(), QHostAddress::Broadcast, 10001);

    // ��������� ������ ��� ��������� broadcast
    broadcastTimer->start();
}

void Server::handlePingTimeout() {
    // ���� ������ 15 ������ ��� �����, ���������� broadcast
    if (isClientConnected) {
        ui->textEdit->append("No pings received in the last 15 seconds. Stopping broadcast.");
        broadcastTimer->stop();
    }
    else {
        ui->textEdit->append("Waiting for client connection...");
    }
}

void Server::handleClientPing() {
    // �������� ��������� ����� �� �������
    if (isClientConnected) {
        ui->textEdit->append("Ping received from client.");

        // �������� ������ � ���, ��� ���� �������
        // ����� ����� �������� ������ ��� ��������� ����� �������
    }
    else {
        ui->textEdit->append("No client connected.");
    }
}
