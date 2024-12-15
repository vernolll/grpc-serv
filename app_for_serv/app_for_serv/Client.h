#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "generated/api.pb.h"
#include "generated/api.grpc.pb.h"

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QObject* parent = nullptr);
    ~Client();

    // ������ ��� ���������� �����������
    void connectToServer(const QString& serverIp, int serverPort);
    void startPinging();
    void stopPinging();

signals:
    void pingReceived(const QString& serverIp);  // ������, ����� ���� ������� �� �������
    void connectionLost(const QString& serverIp);  // ������, ����� ����� � �������� ��������


private slots:
    void processBroadcast();  // ��������� UDP ����������������� ���������
    void sendPing();          // �������� gRPC �����

private:
    // UDP ��� ��������� ����������������� ���������
    QUdpSocket* udpSocket;

    // gRPC ����������� � Stub
    std::shared_ptr<grpc::Channel> grpcChannel;
    std::unique_ptr<MaintainingApi::Stub> grpcStub;

    // ������ ��� �������������� ����������� ������
    QTimer* pingTimer;

    // ������ �������
    QString currentServerIp;
    int currentServerPort;
};

#endif // CLIENT_H
