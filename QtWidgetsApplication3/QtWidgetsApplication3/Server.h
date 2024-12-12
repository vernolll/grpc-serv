#include "ui_QtWidgetsApplication3.h"
#include <QWidget>
#include <QUdpSocket>
#include <QTimer>
#include <QTextEdit>


class Server : public QWidget
{
    Q_OBJECT

public:
    explicit Server(Ui::QtWidgetsApplication3* ui, QWidget* parent = nullptr);
    ~Server();

private slots:
    void onStartButtonClicked();
    void handleClientPing();
    void handlePingTimeout();

private:
    void startBroadcast();

    QUdpSocket* udpSocket;
    QTimer* pingTimer;
    QTimer* broadcastTimer;

    Ui::QtWidgetsApplication3* ui;  // —сылка на ui из главного окна

    int serverPort;
    QString serverIp;
    bool isClientConnected = false;  // ‘лаг, обозначающий подключение клиента
};
