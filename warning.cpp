#include "warning.h"
#include "ui_warning.h"
#include "power.h"

#include <QTimer>

#define FORMAT_MESSAGE		"The system will %1. Save all the work in progress before doing that."

Warning::Warning(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Warning)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::SplashScreen | Qt::WindowStaysOnTopHint);
    this->setFixedSize(this->size());

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_timer()));
    timer->start(1000);

    second = 0;
    ui->progressBar->setRange(0, 30);
    ui->progressBar->setValue(0);
}

Warning::~Warning()
{
    delete ui;
}

void RunAction(Action action, bool force)
{
    switch (action)
    {
    case Action::sleep:
        Power::sleep();
        break;
    case Action::hibernate:
        Power::hibernate();
        break;
    case Action::shutdownnormal:
        Power::shutdown(force);
        break;
    case Action::shutdownhybrid:
        Power::shutdown_hybrib(force);
        break;
    case Action::restart:
        Power::restart(force);
        break;
    case Action::lock:
        Power::lock();
        break;
    case Action::signout:
        Power::signout();
        break;
    case Action::monitoroff:
        Power::monitor_off();
        break;
    }
}

void Warning::on_timer()
{
    ui->progressBar->setValue(++second);
    if (second >= 30)
    {
        on_buttonAction_clicked();
    }
}

void Warning::on_buttonAction_clicked()
{
    //RunAction(action, force);
    this->done(0);
}

void Warning::on_buttonCancel_clicked()
{
    this->done(0);
}

void Warning::getAction(Action action, bool force)
{
    this->action = action;
    this->force = force;
    switch (action)
    {
    case Action::sleep:
        ui->labelMessage->setText(QString(FORMAT_MESSAGE).arg("sleep"));
        ui->buttonAction->setText("Sleep now");
        break;
    case Action::hibernate:
        ui->labelMessage->setText(QString(FORMAT_MESSAGE).arg("hibernate"));
        ui->buttonAction->setText("Hibernate now");
        break;
    case Action::shutdownnormal:
        ui->labelMessage->setText(QString(FORMAT_MESSAGE).arg("shut down"));
        ui->buttonAction->setText("Shut down now");
        break;
    case Action::shutdownhybrid:
        ui->labelMessage->setText(QString(FORMAT_MESSAGE).arg("shut down"));
        ui->buttonAction->setText("Shut down now");
        break;
    case Action::restart:
        ui->labelMessage->setText(QString(FORMAT_MESSAGE).arg("restart"));
        ui->buttonAction->setText("Restart now");
        break;
    case Action::lock:
        ui->labelMessage->setText(QString(FORMAT_MESSAGE).arg("lock"));
        ui->buttonAction->setText("Lock now");
        break;
    case Action::signout:
        ui->labelMessage->setText(QString(FORMAT_MESSAGE).arg("sign out"));
        ui->buttonAction->setText("Sign out now");
        break;
    case Action::monitoroff:
        ui->labelMessage->setText(QString(FORMAT_MESSAGE).arg("turn off monitor"));
        ui->buttonAction->setText("Turn off monitor now");
        break;
    }
}
