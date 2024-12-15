#include "Client.h"
#include <QDebug>
#include <QUdpSocket>
#include <QTimer>
#include <QMessageBox>

Client::Client(QObject* parent)
    : QObject(parent),
    udpSocket(new QUdpSocket(this)),
    pingTimer(new QTimer(this)),
    missedPings(0)
{
    connect(pingTimer, &QTimer::timeout, this, &Client::sendPing);
    connect(udpSocket, &QUdpSocket::readyRead, this, &Client::processResponse);
    udpSocket->bind(QUdpSocket::ShareAddress);  // Binding the socket to listen for UDP packets
}

Client::~Client()
{
    delete udpSocket;
    delete pingTimer;
}

// Этот метод будет вызываться для отправки пинга серверу.
void Client::startPinging(const QString& serverIp, int serverPort)
{
    currentServerIp = serverIp;
    currentServerPort = serverPort;
    missedPings = 0;

    // Показать сообщение о подключении
    QString message = QString("Client is connecting to server at:\nIP: %1\nPort: %2")
        .arg(currentServerIp)
        .arg(currentServerPort);
    QMessageBox::information(nullptr, "Server Connection", message);

    pingTimer->start(5000);  // Отправлять пинг каждые 5 секунд
}

// Остановить пинги.
void Client::stopPinging()
{
    pingTimer->stop();
}

// Отправка пинга серверу
void Client::sendPing()
{
    QByteArray pingMessage = "PING";
    udpSocket->writeDatagram(pingMessage, QHostAddress(currentServerIp), currentServerPort);
    missedPings++;

    if (missedPings > 3) {
        emit connectionLost(currentServerIp);  // Сигнал, если нет ответа на 3 пинга
        stopPinging();
    }
}

// Обработка ответа от сервера
void Client::processResponse()
{
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray response;
        response.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(response.data(), response.size());

        if (response == "PONG") {
            missedPings = 0;
            emit pingReceived(currentServerIp);  // Если пришел ответ "PONG", сбрасываем счетчик пропущенных пингов
        }
    }
}

