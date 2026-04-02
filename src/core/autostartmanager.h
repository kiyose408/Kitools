#ifndef AUTOSTARTMANAGER_H
#define AUTOSTARTMANAGER_H

#include <QObject>
#include <QString>

class AutoStartManager : public QObject
{
    Q_OBJECT

public:
    static AutoStartManager* instance();

    bool isAutoStartEnabled() const;
    void setAutoStartEnabled(bool enabled);

    QString getApplicationPath() const;
    QString getRegistryKeyName() const;

signals:
    void autoStartChanged(bool enabled);

private:
    explicit AutoStartManager(QObject *parent = nullptr);
    ~AutoStartManager();

    AutoStartManager(const AutoStartManager&) = delete;
    AutoStartManager& operator=(const AutoStartManager&) = delete;

    bool checkRegistryAutoStart() const;
    bool writeToRegistry(bool enabled);
};

#endif