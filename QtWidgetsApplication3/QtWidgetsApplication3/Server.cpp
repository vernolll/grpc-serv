#include "Server.h"
#include <QHostAddress>
#include <QUdpSocket>
#include <QTimer>
#include <QTextEdit>

Server::Server(Ui::QtWidgetsApplication3* ui, QWidget* parent) : QWidget(parent), ui(ui) {
    // Подключаем сигналы и слоты
    connect(ui->pushButton, &QPushButton::clicked, this, &Server::onStartButtonClicked);

    // Создаем сокет для UDP
    udpSocket = new QUdpSocket(this);

    // Создаем таймеры
    pingTimer = new QTimer(this);
    broadcastTimer = new QTimer(this);

    // Устанавливаем интервалы
    pingTimer->setInterval(5000);  // Пинг каждую секунду
    broadcastTimer->setInterval(15000);  // Таймаут через 15 секунд

    // Подключаем таймеры к слотам
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
    serverPort = ui->lineEdit->text().toInt(&ok);  // Получаем порт из QLineEdit
    if (!ok || serverPort <= 0) {
        ui->textEdit->append("Invalid port!");  // Если порт неверный, выводим ошибку в QTextEdit
        return;
    }

    serverIp = QHostAddress(QHostAddress::LocalHost).toString();  // Получаем локальный IP

    // Запускаем broadcast
    startBroadcast();

    ui->label->setText("Server Status: Broadcasting...");
    ui->textEdit->append("Broadcasting server info...");

    // Эмуляция подключения клиента
    isClientConnected = true;
    pingTimer->start();  // Начинаем отправлять пинги клиенту
}

void Server::startBroadcast() {
    QString message = serverIp + ":" + QString::number(serverPort);

    // Отправляем сообщение по UDP
    udpSocket->writeDatagram(message.toUtf8(), QHostAddress::Broadcast, 10001);

    // Запускаем таймер для остановки broadcast
    broadcastTimer->start();
}

void Server::handlePingTimeout() {
    // Если прошло 15 секунд без пинга, прекращаем broadcast
    if (isClientConnected) {
        ui->textEdit->append("No pings received in the last 15 seconds. Stopping broadcast.");
        broadcastTimer->stop();
    }
    else {
        ui->textEdit->append("Waiting for client connection...");
    }
}

void Server::handleClientPing() {
    // Эмуляция получения пинга от клиента
    if (isClientConnected) {
        ui->textEdit->append("Ping received from client.");

        // Отправка ответа о том, что пинг успешен
        // Здесь можно обновить статус или отправить ответ клиенту
    }
    else {
        ui->textEdit->append("No client connected.");
    }
}
