#ifndef QT_WIDGETS_APPLICATION3_H
#define QT_WIDGETS_APPLICATION3_H

#include <QMainWindow>
#include "Server.h"
#include "ui_QtWidgetsApplication3.h"

class QtWidgetsApplication3 : public QMainWindow
{
    Q_OBJECT

public:
    explicit QtWidgetsApplication3(QWidget* parent = nullptr);
    ~QtWidgetsApplication3();

private slots:
    void onStartButtonClicked();

private:
    Ui::QtWidgetsApplication3 ui;
    Server* server = nullptr;
};

#endif // QT_WIDGETS_APPLICATION3_H
