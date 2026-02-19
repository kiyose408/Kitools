#ifndef TODODATATYPES_H
#define TODODATATYPES_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

struct TaskData
{
    int id;
    QString description;
    bool isCompleted;
    int completionScore;
    QDateTime creationTime;
    QDateTime completionTime;
    
    TaskData()
        : id(-1)
        , isCompleted(false)
        , completionScore(0)
        , creationTime(QDateTime::currentDateTime())
        , completionTime(QDateTime())
    {}
    
    TaskData(int taskId, const QString &desc)
        : id(taskId)
        , description(desc)
        , isCompleted(false)
        , completionScore(0)
        , creationTime(QDateTime::currentDateTime())
        , completionTime(QDateTime())
    {}
    
    QJsonObject toJson() const
    {
        QJsonObject obj;
        obj["id"] = id;
        obj["description"] = description;
        obj["isCompleted"] = isCompleted;
        obj["completionScore"] = completionScore;
        obj["creationTime"] = creationTime.toString(Qt::ISODate);
        if (completionTime.isValid()) {
            obj["completionTime"] = completionTime.toString(Qt::ISODate);
        }
        return obj;
    }
    
    static TaskData fromJson(const QJsonObject &obj)
    {
        TaskData task;
        task.id = obj["id"].toInt();
        task.description = obj["description"].toString();
        task.isCompleted = obj["isCompleted"].toBool();
        task.completionScore = obj["completionScore"].toInt();
        task.creationTime = QDateTime::fromString(obj["creationTime"].toString(), Qt::ISODate);
        if (obj.contains("completionTime")) {
            task.completionTime = QDateTime::fromString(obj["completionTime"].toString(), Qt::ISODate);
        }
        return task;
    }
};

struct LogEntry
{
    QDateTime timestamp;
    QString action;
    QString taskDescription;
    int score;
    
    LogEntry()
        : timestamp(QDateTime::currentDateTime())
        , score(0)
    {}
    
    LogEntry(const QString &act, const QString &desc, int scr = 0)
        : timestamp(QDateTime::currentDateTime())
        , action(act)
        , taskDescription(desc)
        , score(scr)
    {}
    
    QJsonObject toJson() const
    {
        QJsonObject obj;
        obj["timestamp"] = timestamp.toString(Qt::ISODate);
        obj["action"] = action;
        obj["taskDescription"] = taskDescription;
        obj["score"] = score;
        return obj;
    }
    
    static LogEntry fromJson(const QJsonObject &obj)
    {
        LogEntry entry;
        entry.timestamp = QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate);
        entry.action = obj["action"].toString();
        entry.taskDescription = obj["taskDescription"].toString();
        entry.score = obj["score"].toInt();
        return entry;
    }
};

#endif
