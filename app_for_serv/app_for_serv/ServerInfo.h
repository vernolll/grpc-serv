#pragma once

#include <QPushButton>
#include <QTimer>

struct ServerInfo {
    QString ipPort;
    QString lastPingTime;
    QString status;
    QPushButton* actionButton;
    QTimer* pingTimer;
    int missedPings;
};
