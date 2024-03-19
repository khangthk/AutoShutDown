#include "registry.h"
#include <QSettings>
#include <QCoreApplication>

#define REG_APP_NAME					"AutoShudown"
#define REG_APP_PARENT					"HKEY_CURRENT_USER\\Software"
#define REG_APP_SUBKEY					"HKEY_CURRENT_USER\\Software\\AutoShudown"
#define REG_APP_ACTION_KEY				"Action"
#define REG_APP_SETTING_KEY				"Time"
#define REG_RUN_STARTUP_SUBKEY			"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"

void Registry::AddRunAtStartup()
{
    QSettings settings(REG_RUN_STARTUP_SUBKEY, QSettings::NativeFormat);
    settings.setValue(REG_APP_NAME, QCoreApplication::applicationFilePath());
}

void Registry::RemoveRunAtStartup()
{
    QSettings settings(REG_RUN_STARTUP_SUBKEY, QSettings::NativeFormat);
    settings.remove(REG_APP_NAME);
}

void Registry::AddSetting(Action action, int time)
{
    QSettings settings(REG_APP_SUBKEY, QSettings::NativeFormat);
    settings.setValue(REG_APP_ACTION_KEY, action);
    settings.setValue(REG_APP_SETTING_KEY, time);
}

void Registry::RemoveSetting()
{
    QSettings settings(REG_APP_PARENT, QSettings::NativeFormat);
    settings.remove(REG_APP_NAME);
}

void Registry::ReadSetting(Action &action, int &time)
{
    QSettings settings(REG_APP_SUBKEY, QSettings::NativeFormat);
    action = static_cast<Action>(settings.value(REG_APP_ACTION_KEY, -1).toInt());
    time = settings.value(REG_APP_SETTING_KEY, -1).toInt();
}
