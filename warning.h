#ifndef WARNING_H
#define WARNING_H

#include <QDialog>

#include "power.h"

namespace Ui {
class Warning;
}

class Warning : public QDialog
{
    Q_OBJECT

public:
    explicit Warning(QWidget *parent = nullptr);
    ~Warning();

private slots:
    void on_timer();
    void on_buttonAction_clicked();
    void on_buttonCancel_clicked();
    void getAction(Action, bool);

private:
    Ui::Warning *ui;
    int second;
    QTimer *timer;
    Action action;
    bool force;
};

#endif // WARNING_H
