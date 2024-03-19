#ifndef AUTOSHUTDOWN_H
#define AUTOSHUTDOWN_H

#include <QDialog>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include "power.h"

namespace Ui {
class AutoShutdown;
}

class AutoShutdown : public QDialog
{
    Q_OBJECT

public:
    explicit AutoShutdown(QWidget *parent = nullptr);
    ~AutoShutdown();
protected:
    void closeEvent(QCloseEvent *event);

signals:
    void sendAction(Action, bool);

private slots:
    void on_timer();
    void on_comboBoxAction_currentIndexChanged(int index);
    void on_comboBoxMinutes_editTextChanged(const QString &arg1);
    void on_checkBoxForceAppClose_stateChanged(int arg1);
    void on_radioButtonAfterTime_clicked();
    void on_radioButtonSpecifiedTime_clicked();
    void on_buttonApply_clicked();
    void on_buttonClose_clicked();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void InitComboBoxExAction();
    void InitSettingDateTime();
    void Apply();
    void Stop();
    void ShowWarningDlg();
    void VisibleWidget(bool visible);
    void MinimizeToTray();
    void CreateActions();
    void CreateTrayIcon();

private:
    Ui::AutoShutdown *ui;
    Action currentAction;
    QVector<Action> indexToAction;
    QTimer *timer;
    int timeRemainInSecoond;
    bool forceAppClose;

    QAction *restoreAction;
    QAction *quitAction;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
};

#endif // AUTOSHUTDOWN_H
