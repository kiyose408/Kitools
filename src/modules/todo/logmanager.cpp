#include "logmanager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QTextStream>

LogManager* LogManager::instance()
{
    static LogManager instance;
    return &instance;
}

LogManager::LogManager(QObject *parent)
    : QObject(parent)
{
    loadLogs();
}

LogManager::~LogManager()
{
    saveLogs();
}

QList<LogEntry> LogManager::logs() const
{
    return m_logs;
}

int LogManager::logCount() const
{
    return m_logs.size();
}

void LogManager::logCreate(const QString &taskDescription)
{
    LogEntry entry("创建任务", taskDescription);
    addLog(entry);
}

void LogManager::logComplete(const QString &taskDescription)
{
    LogEntry entry("完成任务", taskDescription);
    addLog(entry);
}

void LogManager::logRate(const QString &taskDescription, int score)
{
    LogEntry entry("评分", taskDescription, score);
    addLog(entry);
}

void LogManager::logDelete(const QString &taskDescription)
{
    LogEntry entry("删除任务", taskDescription);
    addLog(entry);
}

void LogManager::logEdit(const QString &oldDescription, const QString &newDescription)
{
    LogEntry entry("编辑任务", QString("%1 -> %2").arg(oldDescription, newDescription));
    addLog(entry);
}

void LogManager::clearLogs()
{
    m_logs.clear();
    saveLogs();
}

bool LogManager::exportToJson(const QString &filePath)
{
    QJsonArray array;
    for (const LogEntry &entry : m_logs) {
        array.append(entry.toJson());
    }
    
    QJsonDocument doc(array);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    return true;
}

bool LogManager::exportToCsv(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    
    out << "时间,操作,任务描述,评分\n";
    
    for (const LogEntry &entry : m_logs) {
        QString line = QString("%1,%2,%3,%4\n")
            .arg(entry.timestamp.toString("yyyy-MM-dd hh:mm:ss"))
            .arg(entry.action)
            .arg(entry.taskDescription)
            .arg(entry.score);
        out << line;
    }
    
    file.close();
    return true;
}

bool LogManager::importFromJson(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        return false;
    }
    
    QJsonArray array = doc.array();
    for (const QJsonValue &value : array) {
        if (value.isObject()) {
            LogEntry entry = LogEntry::fromJson(value.toObject());
            m_logs.append(entry);
        }
    }
    
    saveLogs();
    return true;
}

void LogManager::loadLogs()
{
    QString filePath = logsFilePath();
    QFile file(filePath);
    
    if (!file.exists()) {
        return;
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        return;
    }
    
    m_logs.clear();
    QJsonArray array = doc.array();
    
    for (const QJsonValue &value : array) {
        if (value.isObject()) {
            LogEntry entry = LogEntry::fromJson(value.toObject());
            m_logs.append(entry);
        }
    }
}

void LogManager::saveLogs()
{
    QString filePath = logsFilePath();
    QDir dir = QFileInfo(filePath).dir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    QJsonArray array;
    for (const LogEntry &entry : m_logs) {
        array.append(entry.toJson());
    }
    
    QJsonDocument doc(array);
    
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

void LogManager::addLog(const LogEntry &entry)
{
    m_logs.append(entry);
    saveLogs();
}

QString LogManager::logsFilePath() const
{
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return dataPath + "/logs.json";
}
