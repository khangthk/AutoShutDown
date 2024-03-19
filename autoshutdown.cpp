#include "autoshutdown.h"
#include "ui_autoshutdown.h"
#include "warning.h"
#include "registry.h"

#include <Windows.h>
#include <VersionHelpers.h>
#include <QTimer>
#include <QMessageBox>
#include <QAction>
#include <QMenu>
#include <QDebug>

#define TIME_FORMAT1		"%1"
#define TIME_FORMAT2		"%1:%2"
#define TIME_FORMAT3		"%1:%2:%3"
#define TIME_FORMAT4		"%1:%2:%3:%4"
#define TIME_FORMAT5		"%1:%2:%3:%4:%5"
#define TIME_FORMAT6		"%1:%2:%3:%4:%5:%6"
#define MAX_VALUE_FORMAT1	(60)
#define MAX_VALUE_FORMAT2	(60*MAX_VALUE_FORMAT1)
#define MAX_VALUE_FORMAT3	(24*MAX_VALUE_FORMAT2)
#define MAX_VALUE_FORMAT4	(30*MAX_VALUE_FORMAT3)
#define MAX_VALUE_FORMAT5	(12*MAX_VALUE_FORMAT4)

typedef struct tagPowerAction {
    Action action;
    QString string;
} PowerAction;

AutoShutdown::AutoShutdown(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoShutdown)
{
    ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setFixedSize(this->size());

    forceAppClose = true;
    ui->checkBoxForceAppClose->setChecked(true);

    ui->comboBoxHours->setValidator(new QIntValidator(0, 99, this));
    ui->comboBoxMinutes->setValidator(new QIntValidator(0, 99, this));

    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_timer()));

    ui->labelTimeRemain->setText("");

    InitComboBoxExAction();
    InitSettingDateTime();

    CreateActions();
    CreateTrayIcon();

    connect(trayIcon, &QSystemTrayIcon::activated, this, &AutoShutdown::iconActivated);
}

AutoShutdown::~AutoShutdown()
{
    delete ui;
}

void AutoShutdown::closeEvent(QCloseEvent *event)
{
    event->ignore();
    MinimizeToTray();
}

void AutoShutdown::on_timer()
{
    // set format
    QString format;
    int formatType;
    if (timeRemainInSecoond <= MAX_VALUE_FORMAT1)
    {
        format = TIME_FORMAT1;
        formatType = 1;
    }
    else if (timeRemainInSecoond <= MAX_VALUE_FORMAT2)
    {
        format = TIME_FORMAT2;
        formatType = 2;
    }
    else if (timeRemainInSecoond <= MAX_VALUE_FORMAT3)
    {
        format = TIME_FORMAT3;
        formatType = 3;
    }
    else if (timeRemainInSecoond <= MAX_VALUE_FORMAT4)
    {
        format = TIME_FORMAT4;
        formatType = 4;
    }
    else if (timeRemainInSecoond <= MAX_VALUE_FORMAT5)
    {
        format = TIME_FORMAT5;
        formatType = 5;
    }
    else
    {
        format = TIME_FORMAT6;
        formatType = 6;
    }

    //display time remain
    QString str;
    int years, months, days, hours, minus, seconds;
    switch (formatType)
    {
        case 1:
        {
            seconds = timeRemainInSecoond;
            str = QString(format).arg(seconds);
            ui->labelTimeRemain->setText(str);
            break;
        }
        case 2:
        {
            minus = timeRemainInSecoond / MAX_VALUE_FORMAT1;
            seconds = timeRemainInSecoond % MAX_VALUE_FORMAT1;
            str = QString(format).arg(minus).arg(seconds);
            ui->labelTimeRemain->setText(str);
            break;
        }
        case 3:
        {
            hours = timeRemainInSecoond / MAX_VALUE_FORMAT2;
            minus = (timeRemainInSecoond % MAX_VALUE_FORMAT2) / MAX_VALUE_FORMAT1;
            seconds = timeRemainInSecoond % MAX_VALUE_FORMAT1;
            str = QString(format).arg(hours).arg(minus).arg(seconds);
            ui->labelTimeRemain->setText(str);
            break;
        }
        case 4:
        {
            days = timeRemainInSecoond / MAX_VALUE_FORMAT3;
            hours = (timeRemainInSecoond % MAX_VALUE_FORMAT3) / MAX_VALUE_FORMAT2;
            minus = (timeRemainInSecoond % MAX_VALUE_FORMAT2) / MAX_VALUE_FORMAT1;
            seconds = timeRemainInSecoond % MAX_VALUE_FORMAT1;
            str = QString(format).arg(days).arg(hours).arg(minus).arg(seconds);
            ui->labelTimeRemain->setText(str);
            break;
        }
        case 5:
        {
            months = timeRemainInSecoond / MAX_VALUE_FORMAT4;
            days = (timeRemainInSecoond % MAX_VALUE_FORMAT4) / MAX_VALUE_FORMAT3;
            hours = (timeRemainInSecoond % MAX_VALUE_FORMAT3) / MAX_VALUE_FORMAT2;
            minus = (timeRemainInSecoond % MAX_VALUE_FORMAT2) / MAX_VALUE_FORMAT1;
            seconds = timeRemainInSecoond % MAX_VALUE_FORMAT1;
            str = QString(format).arg(months).arg(days).arg(hours).arg(minus).arg(seconds);
            ui->labelTimeRemain->setText(str);
            break;
        }
        case 6:
        {
            years = timeRemainInSecoond / MAX_VALUE_FORMAT5;
            months = (timeRemainInSecoond % MAX_VALUE_FORMAT5) / MAX_VALUE_FORMAT4;
            days = (timeRemainInSecoond % MAX_VALUE_FORMAT4) / MAX_VALUE_FORMAT3;
            hours = (timeRemainInSecoond % MAX_VALUE_FORMAT3) / MAX_VALUE_FORMAT2;
            minus = (timeRemainInSecoond % MAX_VALUE_FORMAT2) / MAX_VALUE_FORMAT1;
            seconds = timeRemainInSecoond % MAX_VALUE_FORMAT1;
            str = QString(format).arg(years).arg(months).arg(days).arg(hours).arg(minus).arg(seconds);
            ui->labelTimeRemain->setText(str);
            break;
        }
    }

    if (--timeRemainInSecoond < 0)
    {
        timer->stop();
        ui->labelTimeRemain->setText("");
        ShowWarningDlg();
    }
}

void AutoShutdown::on_comboBoxAction_currentIndexChanged(int index)
{
    currentAction = indexToAction[index];
}

void AutoShutdown::on_comboBoxMinutes_editTextChanged(const QString &arg1)
{
    qDebug() << arg1;
    int minus = arg1.toInt();
    ui->comboBoxMinutes->setCurrentText(QString::number(minus % 60));
    if (minus > 60)
    {
        int currentHours = ui->comboBoxHours->currentText().toInt();
        currentHours += minus / 60;
        ui->comboBoxHours->setCurrentText(QString::number(currentHours));
    }
}

void AutoShutdown::on_checkBoxForceAppClose_stateChanged(int arg1)
{
    forceAppClose = arg1;
}

void AutoShutdown::on_radioButtonAfterTime_clicked()
{
    Registry::RemoveRunAtStartup();
    Registry::RemoveSetting();
}

void AutoShutdown::on_radioButtonSpecifiedTime_clicked()
{
    Registry::RemoveRunAtStartup();
    Registry::RemoveSetting();
}

void AutoShutdown::on_buttonApply_clicked()
{
    if (ui->buttonApply->text().compare("Apply") == 0)
    {
        Apply();
    }
    else
    {
        Stop();
    }
}

void AutoShutdown::on_buttonClose_clicked()
{
    Registry::AddRunAtStartup();
    Registry::RemoveRunAtStartup();
    Action act;
    int time;
    Registry::ReadSetting(act, time);
    Registry::RemoveSetting();
    MinimizeToTray();
}

void AutoShutdown::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        break;
    case QSystemTrayIcon::DoubleClick:
        this->showNormal();
        break;
    default:
        break;
    }
}

void AutoShutdown::InitComboBoxExAction()
{
    ui->comboBoxAction->setIconSize(QSize(24, 24));
    QString itemText[] = { STRING_SLEEP,
                           STRING_HIBERNATE,
                           STRING_SHUTDOWN,
                           STRING_SHUTDOWN_HYBRID,
                           STRING_RESTART,
                           STRING_LOCK,
                           STRING_SIGNOUT,
                           STRING_MONITOR_OFF };
    const int totalItem = sizeof (itemText) / sizeof (*itemText);
    QVector<PowerAction> actions;
    PowerAction action;
    for (int i = 0; i < totalItem; i++)
    {
        action.action = static_cast<Action>(i);
        action.string = itemText[i];
        actions.push_back(action);
    }

    QVector<QIcon> icons;
    icons.push_back(QIcon(":/res/sleep.png"));
    icons.push_back(QIcon(":/res/hibernate.png"));
    icons.push_back(QIcon(":/res/shutdown.png"));
    icons.push_back(QIcon(":/res/shutdown2.png"));
    icons.push_back(QIcon(":/res/restart.png"));
    icons.push_back(QIcon(":/res/lock.png"));
    icons.push_back(QIcon(":/res/signout.png"));
    icons.push_back(QIcon(":/res/monitor.png"));

    int myindex = 0;
    for (int i = 0; i < totalItem; i++)
    {
        if (actions[i].action == Action::hibernate)
        {
            if (!Power::is_support_hibernate())
            {
                continue;
            }
        }
        else if (actions[i].action == Action::shutdownnormal)
        {
            myindex = ui->comboBoxAction->count();
        }
        else if (actions[i].action == Action::shutdownhybrid)
        {
            if (!IsWindows8OrGreater())
            {
                continue;
            }
            else
            {
                currentAction = Action::shutdownhybrid;
                myindex = ui->comboBoxAction->count();
            }
        }
        indexToAction.push_back(actions[i].action);
        ui->comboBoxAction->addItem(icons[i], actions[i].string);
    }
    ui->comboBoxAction->setCurrentIndex(myindex);
}

void AutoShutdown::InitSettingDateTime()
{
    ui->dateEditSpecifiedTime->setDate(QDate::currentDate());
    ui->dateEditSpecifiedTime->setMinimumDate(QDate::currentDate());
    ui->timeEditSpecifiedTime->setTime(QTime::currentTime());
    Action action;
    int time;
    Registry::ReadSetting(action, time);
    if (static_cast<int>(action) == -1 || time == -1)
    {
        ui->timeEditDaily->setTime(QTime::currentTime());
    }
    else
    {
        ui->radioButtonDaily->setChecked(true);
        QTime settingTime(time / MAX_VALUE_FORMAT2, (time % MAX_VALUE_FORMAT2) / MAX_VALUE_FORMAT1, time % MAX_VALUE_FORMAT1);
        qDebug() << time / MAX_VALUE_FORMAT2 << (time % MAX_VALUE_FORMAT2) / MAX_VALUE_FORMAT1 << time % MAX_VALUE_FORMAT1;
        ui->timeEditDaily->setTime(settingTime);
        on_buttonApply_clicked();
    }
}

void AutoShutdown::Apply()
{
    timeRemainInSecoond = 0;
    if (ui->radioButtonAfterTime->isChecked())
    {
        int value = ui->comboBoxMinutes->currentText().toInt();
        timeRemainInSecoond += value * MAX_VALUE_FORMAT1;
        value = ui->comboBoxHours->currentText().toInt();
        timeRemainInSecoond += value * MAX_VALUE_FORMAT2;
        qDebug() << timeRemainInSecoond;
    }
    else if (ui->radioButtonSpecifiedTime->isChecked())
    {
        QDateTime setting = ui->dateEditSpecifiedTime->dateTime();
        setting.setTime(ui->timeEditSpecifiedTime->time());
        QDateTime current = QDateTime::currentDateTime();
        qint64 delta = current.secsTo(setting);
        timeRemainInSecoond = static_cast<int>(delta);
        qDebug() << timeRemainInSecoond;
        if (timeRemainInSecoond < 0)
        {
            QMessageBox::warning(this, "Error", "Your setting time is less than the current time");
            return;
        }
    }
    else
    {
        QTime setting = ui->timeEditDaily->time();
        QTime current = QTime::currentTime();
        int secondOfSettingTime = setting.hour() * MAX_VALUE_FORMAT2 + setting.minute() * MAX_VALUE_FORMAT1 + setting.second();
        int secondOfCurrentTime = current.hour() * MAX_VALUE_FORMAT2 + current.minute() * MAX_VALUE_FORMAT1 + current.second();
        if (secondOfCurrentTime <= secondOfSettingTime)
        {
            timeRemainInSecoond = secondOfSettingTime - secondOfCurrentTime;
        }
        else
        {
            timeRemainInSecoond = 24 * 60 * 60 - (secondOfCurrentTime - secondOfSettingTime);
        }
        qDebug() << timeRemainInSecoond;
        Registry::AddRunAtStartup();
        Registry::AddSetting(currentAction, secondOfSettingTime);
    }

    timer->start();
    ui->buttonApply->setText("Stop");
    ui->labelTimeRemain->setText("");
    VisibleWidget(false);
}

void AutoShutdown::Stop()
{
    timer->stop();
    ui->buttonApply->setText("Apply");
    ui->labelTimeRemain->setText("");
    VisibleWidget(true);
}

void AutoShutdown::ShowWarningDlg()
{
    Warning *warning = new Warning(this);
    connect(this, SIGNAL(sendAction(Action, bool)), warning, SLOT(getAction(Action, bool)));
    emit sendAction(currentAction, forceAppClose);
    int ret = warning->exec();
    if (ret == 0) Stop();
}

void AutoShutdown::VisibleWidget(bool visible)
{
    if (visible)
    {
        ui->groupBoxAction->setEnabled(true);
        ui->groupBoxTime->setEnabled(true);
    }
    else
    {
        ui->groupBoxAction->setEnabled(false);
        ui->groupBoxTime->setEnabled(false);
    }
}

void AutoShutdown::MinimizeToTray()
{
    this->hide();
}

void AutoShutdown::CreateActions()
{
    restoreAction = new QAction(tr("&Open"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void AutoShutdown::CreateTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    QIcon icon = QIcon(":/autoshutdown.ico");
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(icon);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();
}
