#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QObject* parent = nullptr);
    ~Client();

    void startPinging(const QString& serverIp, int serverPort);  // ������ �����
    void stopPinging();  // ��������� �����

signals:
    void pingReceived(const QString& ipPort);  // ������ � ��������� �����
    void connectionLost(const QString& ipPort);  // ������ � ������ ����������
    void serverDiscovered(const QString& ipPort);  // ������ �� ����������� �������

private slots:
    void sendPing();  // �������� ����� �������
    void processResponse();  // ��������� ������ �� �������

private:
    QUdpSocket* udpSocket;  // UDP ����� ��� ����� � ��������
    QTimer* pingTimer;  // ������ ��� �������� �����
    int missedPings;  // ������� ����������� ������
    QString currentServerIp;  // IP �������� �������
    int currentServerPort;  // ���� �������� �������

    void processBroadcast();  // ��������� ����������������� �������
};

#endif // CLIENT_H
