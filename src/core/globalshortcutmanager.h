#ifndef GLOBALSHORTCUTMANAGER_H
#define GLOBALSHORTCUTMANAGER_H

#include <QObject>
#include <QKeySequence>
#include <QMap>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

class GlobalShortcutManager : public QObject {
    Q_OBJECT

public:
    static GlobalShortcutManager* instance(QObject* parent = nullptr);

    bool registerShortcut(const QString& id, const QKeySequence& shortcut);
    void unregisterShortcut(const QString& id);
    void updateShortcut(const QString& id, const QKeySequence& shortcut);

signals:
    void shortcutTriggered(const QString& id);
    void shortcutTriggeredById(int nativeId);

public slots:
    void onShortcutTriggeredById(int nativeId);

private:
    explicit GlobalShortcutManager(QObject* parent = nullptr);
    ~GlobalShortcutManager();

    static GlobalShortcutManager* m_instance;
    
    struct ShortcutInfo {
        int id;
        quint32 nativeKey;
        quint32 nativeModifiers;
    };
    
    QMap<QString, ShortcutInfo> m_shortcuts;
    QMap<int, QString> m_idMap;
    int m_nextId;
    
    int getNextId();
#ifdef Q_OS_WIN
    quint32 nativeKeyFromQt(Qt::Key key);
    quint32 nativeModifierFromQt(Qt::Key key);
#endif
};

#endif
