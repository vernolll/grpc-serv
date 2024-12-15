#include "Client.h"
#include <QDebug>

Client::Client(QObject* parent)
    : QObject(parent), udpSocket(new QUdpSocket(this)), pingTimer(new QTimer(this)), missedPings(0)
{
    connect(pingTimer, &QTimer::timeout, this, &Client::sendPing);
    connect(udpSocket, &QUdpSocket::readyRead, this, &Client::processResponse);
    udpSocket->bind(QUdpSocket::ShareAddress);
}

Client::~Client()
{
    delete udpSocket;
    delete pingTimer;
}

void Client::startPinging(const QString& serverIp, int serverPort)
{
    currentServerIp = serverIp;
    currentServerPort = serverPort;
    missedPings = 0;

    pingTimer->start(5000);
}

void Client::stopPinging()
{
    pingTimer->stop();
}

void Client::sendPing()
{
    QByteArray pingMessage = "PING";
    udpSocket->writeDatagram(pingMessage, QHostAddress(currentServerIp), currentServerPort);

    missedPings++;
    if (missedPings > 3) {
        emit connectionLost(currentServerIp);
        stopPinging();
    }
}

void Client::processResponse()
{
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray response;
        response.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(response.data(), response.size());

        if (response == "PONG") {
            missedPings = 0;
            emit pingReceived(currentServerIp);
        }
    }
}

void Client::processBroadcast()
{
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());

        emit serverDiscovered(QString::fromUtf8(datagram));
    }
}