#include "autostartmanager.h"
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#include <shlobj.h>
#endif

AutoStartManager* AutoStartManager::instance()
{
    static AutoStartManager instance;
    return &instance;
}

AutoStartManager::AutoStartManager(QObject *parent)
    : QObject(parent)
{
}

AutoStartManager::~AutoStartManager()
{
}

QString AutoStartManager::getRegistryKeyName() const
{
    return "PCProductivityToolbox";
}

QString AutoStartManager::getApplicationPath() const
{
    QString appPath = QCoreApplication::applicationFilePath();
    appPath = QDir::toNativeSeparators(appPath);
    return appPath;
}

bool AutoStartManager::isAutoStartEnabled() const
{
    return checkRegistryAutoStart();
}

void AutoStartManager::setAutoStartEnabled(bool enabled)
{
    bool currentStatus = checkRegistryAutoStart();
    
    if (currentStatus == enabled) {
        qDebug() << "自启动状态未改变:" << (enabled ? "已启用" : "已禁用");
        return;
    }

    bool success = writeToRegistry(enabled);
    
    if (success) {
        qDebug() << "自启动设置成功:" << (enabled ? "已启用" : "已禁用");
        emit autoStartChanged(enabled);
    } else {
        qWarning() << "自启动设置失败!";
    }
}

bool AutoStartManager::checkRegistryAutoStart() const
{
#ifdef Q_OS_WIN
    HKEY hKey;
    LONG result = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0,
        KEY_READ,
        &hKey
    );

    if (result != ERROR_SUCCESS) {
        qDebug() << "无法打开注册表键,错误代码:" << result;
        return false;
    }

    wchar_t value[512];
    DWORD valueSize = sizeof(value);
    DWORD type;

    QString keyName = getRegistryKeyName();
    result = RegQueryValueExW(
        hKey,
        keyName.toStdWString().c_str(),
        nullptr,
        &type,
        reinterpret_cast<LPBYTE>(value),
        &valueSize
    );

    RegCloseKey(hKey);

    if (result == ERROR_SUCCESS) {
        QString regValue = QString::fromWCharArray(value);
        QString currentPath = getApplicationPath();
        qDebug() << "注册表中存在自启动项,值:" << regValue;
        qDebug() << "当前应用路径:" << currentPath;
        return !regValue.isEmpty();
    }

    qDebug() << "注册表中不存在自启动项";
    return false;
#else
    return false;
#endif
}

bool AutoStartManager::writeToRegistry(bool enabled)
{
#ifdef Q_OS_WIN
    HKEY hKey;
    LONG result = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0,
        KEY_SET_VALUE,
        &hKey
    );

    if (result != ERROR_SUCCESS) {
        qWarning() << "无法打开注册表进行写入,错误代码:" << result;
        return false;
    }

    QString keyName = getRegistryKeyName();

    if (enabled) {
        QString appPath = getApplicationPath();
        std::wstring widePath = appPath.toStdWString();
        std::wstring wideKeyName = keyName.toStdWString();

        result = RegSetValueExW(
            hKey,
            wideKeyName.c_str(),
            0,
            REG_SZ,
            reinterpret_cast<const BYTE*>(widePath.c_str()),
            static_cast<DWORD>((widePath.size() + 1) * sizeof(wchar_t))
        );

        if (result == ERROR_SUCCESS) {
            qDebug() << "已添加到注册表自启动项,路径:" << appPath;
        } else {
            qWarning() << "写入注册表失败,错误代码:" << result;
        }
    } else {
        std::wstring wideKeyName = keyName.toStdWString();
        result = RegDeleteValueW(hKey, wideKeyName.c_str());

        if (result == ERROR_SUCCESS) {
            qDebug() << "已从注册表删除自启动项";
        } else if (result == ERROR_FILE_NOT_FOUND) {
            qDebug() << "注册表中不存在该自启动项(可能已被删除)";
            result = ERROR_SUCCESS;
        } else {
            qWarning() << "删除注册表值失败,错误代码:" << result;
        }
    }

    RegCloseKey(hKey);
    return (result == ERROR_SUCCESS);
#else
    Q_UNUSED(enabled);
    return false;
#endif
}