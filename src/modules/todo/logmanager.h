#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QObject>
#include <QList>
#include "tododatatypes.h"

class LogManager : public QObject
{
    Q_OBJECT

public:
    static LogManager* instance();
    
    QList<LogEntry> logs() const;
    int logCount() const;
    
    void logCreate(const QString &taskDescription);
    void logComplete(const QString &taskDescription);
    void logRate(const QString &taskDescription, int score);
    void logDelete(const QString &taskDescription);
    void logEdit(const QString &oldDescription, const QString &newDescription);
    
    void clearLogs();
    
    bool exportToJson(const QString &filePath);
    bool exportToCsv(const QString &filePath);
    bool importFromJson(const QString &filePath);
    
    void loadLogs();
    void saveLogs();

signals:
    void logAdded();
    void logsCleared();

private:
    explicit LogManager(QObject *parent = nullptr);
    ~LogManager();
    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;
    
    void addLog(const LogEntry &entry);
    QString logsFilePath() const;

    QList<LogEntry> m_logs;
};

#endif
